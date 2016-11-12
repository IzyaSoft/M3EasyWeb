#include "networkService.h"
#include "arp.h"
#include "ip.h"
#include "networkConfiguration.h"
#include "networkUtils.h"
#include "debugPrintFunctions.h"

extern struct NetworkConfiguration networkConfiguration;

unsigned char* arpCache[6];

static void WriteData(struct EthernetBuffer* txBuffer, unsigned char debugPrintEnabled)
{
	if(debugPrintEnabled)
	{
        printf("Outgoing Packet: ");
        printStringHexSymbols(txBuffer->_buffer, txBuffer->_storedBytes, 6);
	}
    Write(txBuffer);
}

static void ReadData(struct EthernetBuffer* rxBuffer, unsigned char debugPrintEnabled)
{
    Read(rxBuffer);
    if(debugPrintEnabled)
    {
        printf("Incoming Packet: ");
        printStringHexSymbols(rxBuffer->_buffer, rxBuffer->_storedBytes, 6);
    }
}

void TransmitData(struct EthernetBuffer* buffer)
{
	WriteData(buffer, 0);
}

//todo: umv: make proper arp cache
void InitializeNetwork(struct EthernetConfiguration* ethernetConfiguration)
{
    InitializeEthrernet(ethernetConfiguration);
}

// Network Packet Routing
void HandleNetworkEvents()
{
    if(CheckIsDataAvailable())
    {
        uint32_t dataSize = CheckAvailableDataSize();
        //unsigned char localBuffer[MAX_LOCAL_BUFFER_SIZE_LIMIT] = {};  // todo: umv possibly do malloc + free
        struct EthernetBuffer rxBuffer;
        rxBuffer._buffer = ethernetBuffer;
        rxBuffer._bufferCapacity = MAX_ETH_FRAME_SIZE;
/*        if(dataSize <= MAX_LOCAL_BUFFER_SIZE_LIMIT)
        {
            // local buffer usage
            rxBuffer._buffer = localBuffer;
            rxBuffer._bufferCapacity = MAX_LOCAL_BUFFER_SIZE_LIMIT;
        }
        else
        {
            rxBuffer._buffer = ethernetBuffer;
            rxBuffer._bufferCapacity = MAX_ETH_FRAME_SIZE;
        }*/

        ReadData(&rxBuffer, 0);
        if(CheckIsPacketBrodcast(&rxBuffer))
        {
            HandleBrodcastPacket(&rxBuffer);
        }
        else
        {
            HandleIndividualAddressPacket(&rxBuffer);
        }

    }
}

unsigned char CheckIsPacketBrodcast(struct EthernetBuffer* buffer)
{
    for(unsigned char counter = 0; counter < MAC_ADDRESS_LENGTH; counter++)
        if(buffer->_buffer[counter] != 0xFF)
            return 0;
    return 1;
}

void HandleBrodcastPacket(struct EthernetBuffer* buffer)
{
    unsigned short etherType = GetEtherType(buffer);
    if(etherType == ARP_ETHERTYPE)
    {
        BuildArpReply(buffer);
        WriteData(buffer, 0);
    }
}

void HandleIndividualAddressPacket(struct EthernetBuffer* buffer)
{
    unsigned short etherType = GetEtherType(buffer);
    if(etherType == ARP_ETHERTYPE)
    {
        if(GetWord(buffer, ARP_OPCODE_INDEX) == ARP_REPLY_OPERATION)
        {
            //todo: umv: make arp table ip, MAC, counter
            memcpy(arpCache, &buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
        }
    }

    if(etherType == IP_ETHERTYPE)
    {
        if((GetWord(buffer, ETHERNET_PAYLOAD_INDEX) & 0xFF00) == IPV4_VERSION)
        {
            if(! (GetWord(buffer, IP_PACKET_FLAGS_INDEX) & (IP_FLAG_MOREFRAG | IP_FRAGOFS_MASK)))
            {
                switch(buffer->_buffer[IP_PACKET_PROTOCOL_INDEX])
                {
                    case ICMP_PROTOCOL:
                         BuildIcmpPacket(buffer);
                         Write(buffer);
                         break;
                    case TCP_PROTOCOL:
                         HandleTcpPacket(buffer);
                         break;
                    case UDP_PROTOCOL:
                         break;
                }
            }
        }
    }
}
