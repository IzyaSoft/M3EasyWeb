#include "arp.h"
#include "ethernet.h"
#include "networkManager.h"
#include "networkUtils.h"

extern struct NetworkConfiguration networkConfiguration;
extern unsigned char arpCache[6] = {};

/*static void RevertMacAddress(unsigned char* macAddress)
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
}*/

// todo: umv: very rough method, make it more smarter
static unsigned char GetNetmaskFfBytesNumber(unsigned char* netmask)
{
unsigned char ffBytes = 0;
    for(unsigned char counter = 0; counter < IPV4_LENGTH; counter++)
    {
        if(netmask[counter] == 0xFF)
            ffBytes++;
        else break;
    }
    return ffBytes;
}

static void BuildArpPacketImpl(struct EthernetBuffer* buffer, unsigned char operationType,
                               unsigned char* sourceMac, unsigned char* destinationMac,
                               unsigned char* sourceIpAddress, unsigned char* destinationIpAddress)
{
	// Ethernet
    InsertEthernetHeader(buffer, sourceMac, destinationMac, ARP_ETHERTYPE);
    // ARP
    SetWord(HARDWARE_ETH10, buffer, ARP_HARDWARE_ADDRESS_TYPE_INDEX);
    SetWord(IP_ETHERTYPE, buffer, ARP_PROTOCOL_INDEX);
    SetWord(IPV4_HLEN_PLEN, buffer, ARP_HLEN_PLEN_INDEX);
    SetWord(operationType, buffer, ARP_OPCODE_INDEX);
    memcpy(&buffer->_buffer[ARP_SENDER_MAC_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ARP_SENDER_IP_INDEX], sourceIpAddress, IPV4_LENGTH);
    memcpy(&buffer->_buffer[ARP_TARGET_MAC_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&buffer->_buffer[ARP_TARGET_IP_INDEX], destinationIpAddress, IPV4_LENGTH);
    // Set length in bytes
    buffer->_storedBytes = ETHERNET_HEADER_SIZE + ARP_PACKET_SIZE;
}

void BuildArpReply(struct EthernetBuffer* buffer)
{
    unsigned char destinationMac[MAC_ADDRESS_LENGTH];
    unsigned char destinationIpAddress[IPV4_LENGTH];

    memcpy(destinationMac, &buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
    memcpy(destinationIpAddress, &buffer->_buffer[ARP_SENDER_IP_INDEX], IPV4_LENGTH);

    BuildArpPacketImpl(buffer, ARP_REPLY_OPERATION, networkConfiguration._revertedMacAddress, destinationMac, networkConfiguration._ipAddress, destinationIpAddress);
}

void BuildArpRequest(struct EthernetBuffer* buffer, unsigned char* destinationIpAddress)
{
    unsigned char destinationMac[MAC_ADDRESS_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    unsigned char ffBytesNumber = GetNetmaskFfBytesNumber(networkConfiguration._netmask);
    unsigned char useGatewayAsDestination = 0;
    for(unsigned char counter = 0; counter < ffBytesNumber; counter ++)
    {
        if(networkConfiguration._ipAddress[counter] != destinationIpAddress[counter])
        {
            useGatewayAsDestination = 1;
            break;
        }
    }

    BuildArpPacketImpl(buffer, ARP_REQUEST_OPERATION, networkConfiguration._revertedMacAddress, destinationMac, networkConfiguration._ipAddress, useGatewayAsDestination ? networkConfiguration._gateway : destinationIpAddress);
}
