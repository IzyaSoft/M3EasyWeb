#include "hal.h"
#ifdef LANDTIGER2
#include "ethernetDriver.h"
#include "LPC17xx.h"

#ifdef USE_ETHERNET_DRIVER

    static int32_t WritePhy (int32_t phyReg, int32_t value)
    {
        uint32_t tout = 0;

        LPC_EMAC->MADR = DP83848C_DEF_ADR | phyReg;
        LPC_EMAC->MWTD = value;

        /* Wait until operation completed */
        for (tout = 0; tout < MII_WR_TOUT; tout++)
        {
            if ((LPC_EMAC->MIND & MIND_BUSY) == 0)
                return 0;
        }
        return -1;
    }

    static int32_t ReadPhy (unsigned char phyReg)
    {
        uint32_t tout = 0;

        LPC_EMAC->MADR = DP83848C_DEF_ADR | phyReg;
        LPC_EMAC->MCMD = MCMD_READ;

        /* Wait until operation completed */
        for (tout = 0; tout < MII_RD_TOUT; tout++)
        {
            if((LPC_EMAC->MIND & MIND_BUSY) == 0)
            {
                LPC_EMAC->MCMD = 0;
                return (LPC_EMAC->MRDD);
            }
        }
        return -1;
    }

    static void RxDescriptorInit ()
    {
        /* Initialize Receive Descriptor and Status array. */
        for (uint32_t i = 0; i < NUM_RX_FRAG; i++)
        {
            RX_DESC_PACKET(i) = RX_BUF(i);
            RX_DESC_CTRL(i) = RCTRL_INT | (ETH_FRAG_SIZE-1);
            RX_STAT_INFO(i) = 0;
            RX_STAT_HASHCRC(i) = 0;
        }

        LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG - 1;
        LPC_EMAC->RxDescriptor = RX_DESC_BASE;
        LPC_EMAC->RxStatus = RX_STAT_BASE;

        /* Rx Descriptors Point to 0 */
        LPC_EMAC->RxConsumeIndex  = 0;
    }

    static uint32_t GetReceivedDataSize()
    {
        uint32_t dmaBufferIndex;
        dmaBufferIndex = LPC_EMAC->RxConsumeIndex;
        return (RX_STAT_INFO(dmaBufferIndex) & RINFO_SIZE);
    }

    void TxDescriptorInit ()
    {
        for (uint32_t i = 0; i < NUM_TX_FRAG; i++)
        {
            TX_DESC_PACKET(i) = TX_BUF(i);
            TX_DESC_CTRL(i) = 0;
            TX_STAT_INFO(i) = 0;
        }

      /* Set EMAC Transmit Descriptor Registers. */
      LPC_EMAC->TxDescriptor = TX_DESC_BASE;
      LPC_EMAC->TxStatus = TX_STAT_BASE;
      LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

      /* Tx Descriptors Point to 0 */
      LPC_EMAC->TxProduceIndex  = 0;
    }

    unsigned char InitializeEthrernet(struct EthernetConfiguration* configuration)
    {
        // Initializes the EMAC ethernet controller
        uint32_t regv, tout, id1, id2;

        /* Power Up the EMAC controller. */
        LPC_SC->PCONP |= 0x40000000;

        /* For the first silicon rev.'-' ID P1.6 should be set, but on rev. 'A' and later, P1.6 should NOT be set. */
        LPC_PINCON->PINSEL2 = 0x50150105;

        LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

        /* Reset all EMAC internal modules. */
        LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
        // MAC_COMMAND = CR_REG_RES | CR_TX_RES | CR_RX_RES;
        LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;

        /* A short delay after reset. */
        for (tout = 100; tout; tout--);

        /* Initialize MAC control registers. */
        LPC_EMAC->MAC1 = MAC1_PASS_ALL;
        LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
        //  MAC2 = MAC2_CRC_EN | MAC2_PAD_EN | MAC2_VLAN_PAD_EN;

        LPC_EMAC->MAXF = ETH_MAX_FLEN;
        LPC_EMAC->CLRT = CLRT_DEF;
        LPC_EMAC->IPGR = IPGR_DEF;

        /* Enable Reduced MII interface. */
        LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM;

        /* Put the DP83848C in reset mode */
        WritePhy (PHY_REG_BMCR, 0x8000);

        /* Wait for hardware reset to end. */
        for (tout = 0; tout < 0x100000; tout++)
        {
            regv = read_PHY(PHY_REG_BMCR);
            if (!(regv & 0x8000))
                break;    /* Reset complete */
        }

        if(regv & 0x8000)
            return 0;

        /* Check if this is a DP83848C PHY. */
        id1 = ReadPhy(PHY_REG_IDR1);
        id2 = ReadPhy(PHY_REG_IDR2);

        if (((id1 << 16) | (id2 & 0xFFF0)) == DP83848C_ID)
        {
            /* Configure the PHY device */

            /* Use autonegotiation about the link speed. */
            WritePhy(PHY_REG_BMCR, PHY_AUTO_NEG);
            /* Wait to complete Auto_Negotiation. */
            for (tout = 0; tout <0x100000; tout++)
            {
                regv = read_PHY(PHY_REG_BMSR);
                if (regv & 0x0020)
                    break; /* Autonegotiation Complete. */
            }
        }

        //if (!(regv & 0x0020))
            //return 0;

        /* Check the link status. */
        for (tout = 0; tout < 0x10000; tout++)
        {
            regv = read_PHY(PHY_REG_STS);
            if (regv & 0x0001)
                break;/* Link is on. */
        }

        //if(!(regv & 0x0001))
            //return 0;

        // todo: umv : settings for speed mode & channel mode from parameter

        /* Configure Full/Half Duplex mode. */
        if (regv & 0x0004)
        {
            /* Full duplex is enabled. */
            LPC_EMAC->MAC2 |= MAC2_FULL_DUP;
            LPC_EMAC->Command |= CR_FULL_DUP;
            LPC_EMAC->IPGT = IPGT_FULL_DUP;
        }
        else LPC_EMAC->IPGT = IPGT_HALF_DUP;/* Half duplex mode. */

        /* Configure 100MBit/10MBit mode. */
        if (regv & 0x0002)
        {
            /* 10MBit mode. */
            LPC_EMAC->SUPP = 0;
        }
        else
        {
            /* 100MBit mode. */
            LPC_EMAC->SUPP = SUPP_SPEED;
        }

        /* Set the Ethernet MAC Address registers */
        LPC_EMAC->SA0 = (configuration->_macAddress[0] << 8) | configuration->_macAddress[1];
        LPC_EMAC->SA1 = (configuration->_macAddress[2] << 8) | configuration->_macAddress[3];
        LPC_EMAC->SA2 = (configuration->_macAddress[4] << 8) | configuration->_macAddress[5];

        /* Initialize Tx and Rx DMA Descriptors */
        RxDescriptorInit();
        TxDescriptorInit();

        /* Receive Broadcast and Perfect Match Packets */
        LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

        /* Enable EMAC interrupts. */
        LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

        /* Reset all interrupts */
        LPC_EMAC->IntClear  = 0xFFFF;

        /* Enable receive and transmit mode of MAC Ethernet core */
        LPC_EMAC->Command |= (CR_RX_EN | CR_TX_EN);
        LPC_EMAC->MAC1 |= MAC1_REC_EN;
        return 1;
    }

    void Read(struct EthernetBuffer* readBuffer)
    {
        uint32_t dmaBufferIndex;
        uint32_t length;
        unsigned char* *destination;
        unsigned char* *source;

        dmaBufferIndex = LPC_EMAC->RxConsumeIndex;
        destination = (unsigned char*) readBuffer->_buffer;
        source = (unsigned char*) RX_BUF(dmaBufferIndex);

        length = (GetReceivedDataSize() - 3) >> 2;
        readBuffer->_storedBytes = length;

        if (readBuffer->_buffer != 0)
        {
            while(length -- > 0)
                *destination++ = *source++;
        }
    }

    void Write(struct EthernetBuffer* bufferToWrite)
    {
        uint32_t dmaBufferIndex;
        unsigned char* source;
        unsigned char* destination;

        dmaBufferIndex = LPC_EMAC->TxProduceIndex;
        source = (unsigned char*)bufferToWrite->_buffer;
        destination  = (unsigned char*)TX_BUF(dmaBufferIndex);
        TX_DESC_CTRL(dmaBufferIndex) = bufferToWrite->_storedBytes | TCTRL_LAST | TCTRL_CRC;

        for(uint32_t ethOctet = 0; ethOctet < bufferToWrite->_storedBytes; ethOctet++)
            *destination++ = *source++;

        dmaBufferIndex = LPC_EMAC->TxProduceIndex;
        if (++dmaBufferIndex == NUM_TX_FRAG)
        dmaBufferIndex = 0;
        LPC_EMAC->TxProduceIndex = dmaBufferIndex;
    }

    uint32_t GetPhyStatus(uint32_t parameter)
    {
        return 0;
    }

#endif

#endif
