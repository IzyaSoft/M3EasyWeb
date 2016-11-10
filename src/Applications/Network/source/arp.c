#include "arp.h"
#include "ethernet.h"

static void RevertMacAddress(unsigned char* macAddress)
{
    unsigned char swap = macAddress[0];
    macAddress[0] = macAddress[5];
    macAddress[5] = swap;
    swap= macAddress[1];
    macAddress[1] = macAddress[4];
    macAddress[4] = swap;
    swap = macAddress[2];
    macAddress[2] = macAddress[3];
    macAddress[3] = swap;
}

void BuildArpReply(struct EthernetBuffer* buffer, unsigned char* macAddress, unsigned char* ipAddress)
{
    unsigned char destinationMac[MAC_ADDRESS_LENGTH];
    unsigned char destinatioIpAddress[IPV4_LENGTH];
    unsigned char sourceMac[MAC_ADDRESS_LENGTH];

    /*printf("Device MAC: ");
    printStringHexSymbols(macAddress, 6, 0);
    printf("Device IP: ");
    printStringHexSymbols(ipAddress, 4, 0);*/

    // copy before buffer re-writing
    memcpy(sourceMac, macAddress, MAC_ADDRESS_LENGTH);
    memcpy(destinationMac, &buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
    memcpy(destinatioIpAddress, &buffer->_buffer[ARP_SENDER_IP_INDEX], IPV4_LENGTH);
    RevertMacAddress(sourceMac);

    //printf("Device MAC: ");
    //printStringHexSymbols(sourceMac, 6, 0);
    // Ethernet
    memcpy(&buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
    //todo: umv: wtf??? find more logic scheme
    *(unsigned short *)&buffer->_buffer[ETHERNET_ETHERTYPE_INDEX] = SWAPBYTES(ARP_ETHERTYPE);
    // ARP
    *(unsigned short *)&buffer->_buffer[ARP_HARDWARE_ADDRESS_TYPE_INDEX] = SWAPBYTES(HARDWARE_ETH10);
    *(unsigned short *)&buffer->_buffer[ARP_PROTOCOL_INDEX] = SWAPBYTES(IP_ETHERTYPE);
    *(unsigned short *)&buffer->_buffer[ARP_HLEN_PLEN_INDEX] = SWAPBYTES(IPV4_HLEN_PLEN);
    *(unsigned short *)&buffer->_buffer[ARP_OPCODE_INDEX] = SWAPBYTES(ARP_REPLY_OPERATION);
    memcpy(&buffer->_buffer[ARP_SENDER_MAC_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ARP_SENDER_IP_INDEX], ipAddress, IPV4_LENGTH);
    memcpy(&buffer->_buffer[ARP_TARGET_MAC_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ARP_TARGET_IP_INDEX], destinatioIpAddress, IPV4_LENGTH);
    // Set length in bytes
    buffer->_storedBytes = ETHERNET_HEADER_SIZE + ARP_PACKET_SIZE;
}

void BuildArpRequest(struct EthernetBuffer* buffer)
{

}
