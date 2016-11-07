#include "arp.h"
#include "ethernet.h"

void BuildArpReply(struct EthernetBuffer* buffer, unsigned char* macAddress, unsigned char* ipAddress)
{
    unsigned char* destinationMac[MAC_ADDRESS_LENGTH];
    unsigned char* destinatioIpAddress[IPV4_LENGTH];

    // copy before buffer re-writing
    memcpy(destinationMac, buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
    memcpy(destinationMac, buffer->_buffer[ARP_SENDER_IP_INDEX], IPV4_LENGTH);
    // Ethernet
    memcpy(buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);

    memcpy(buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
    // ARP
    /*memcpy(&TxFrame2[ETH_DA_OFS], &RecdFrameMAC, 6);
    memcpy(&TxFrame2[ETH_SA_OFS], &MyMAC, 6);
    *(unsigned short *)&TxFrame2[ETH_TYPE_OFS] = SWAPB(FRAME_ARP);

    // ARP
    *(unsigned short *)&TxFrame2[ARP_HARDW_OFS] = SWAPB(HARDW_ETH10);
    *(unsigned short *)&TxFrame2[ARP_PROT_OFS] = SWAPB(FRAME_IP);
    *(unsigned short *)&TxFrame2[ARP_HLEN_PLEN_OFS] = SWAPB(IP_HLEN_PLEN);
    *(unsigned short *)&TxFrame2[ARP_OPCODE_OFS] = SWAPB(OP_ARP_ANSWER);
    memcpy(&TxFrame2[ARP_SENDER_HA_OFS], &MyMAC, 6);
    memcpy(&TxFrame2[ARP_SENDER_IP_OFS], &MyIP, 4);
    memcpy(&TxFrame2[ARP_TARGET_HA_OFS], &RecdFrameMAC, 6);,
    memcpy(&TxFrame2[ARP_TARGET_IP_OFS], &RecdFrameIP, 4);

    TxFrame2Size = ETH_HEADER_SIZE + ARP_FRAME_SIZE;
    TransmitControl |= SEND_FRAME2;*/
}
