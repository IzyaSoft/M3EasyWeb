#include "arp.h"
#include "ethernet.h"
#include "networkManager.h"
#include "networkUtils.h"

extern struct NetworkConfiguration networkConfiguration;
struct EthernetBuffer txBuffer;
extern unsigned char localBuffer[SMALL_FRAME_SIZE];

// todo: umv: very rough method, make it more smarter
static unsigned char GetNetmaskFFBytesNumber(unsigned char* netmask)
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

static void BuildArpPacketImpl(unsigned char operationType, unsigned char* sourceMac, unsigned char* destinationMac, unsigned char* sourceIpAddress, unsigned char* destinationIpAddress)
{
    txBuffer._buffer = localBuffer;
    txBuffer._bufferCapacity = SMALL_FRAME_SIZE;

	// Ethernet
    InsertEthernetHeader(&txBuffer, sourceMac, destinationMac, ARP_ETHERTYPE);
    // ARP
    SetWord(HARDWARE_ETH10, &txBuffer, ARP_HARDWARE_ADDRESS_TYPE_INDEX);
    SetWord(IP_ETHERTYPE, &txBuffer, ARP_PROTOCOL_INDEX);
    SetWord(IPV4_HLEN_PLEN, &txBuffer, ARP_HLEN_PLEN_INDEX);
    SetWord(operationType, &txBuffer, ARP_OPCODE_INDEX);
    memcpy(&txBuffer._buffer[ARP_SENDER_MAC_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
    memcpy(&txBuffer._buffer[ARP_SENDER_IP_INDEX], sourceIpAddress, IPV4_LENGTH);
    memcpy(&txBuffer._buffer[ARP_TARGET_MAC_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
    memcpy(&txBuffer._buffer[ARP_TARGET_IP_INDEX], destinationIpAddress, IPV4_LENGTH);
    // Set length in bytes
    txBuffer._storedBytes = ETHERNET_HEADER_SIZE + ARP_PACKET_SIZE;
}

struct EthernetBuffer* BuildArpReply(struct EthernetBuffer* buffer)
{
    unsigned char destinationMac[MAC_ADDRESS_LENGTH];
    unsigned char destinationIpAddress[IPV4_LENGTH];

    memcpy(destinationMac, &buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
    memcpy(destinationIpAddress, &buffer->_buffer[ARP_SENDER_IP_INDEX], IPV4_LENGTH);
    BuildArpPacketImpl(ARP_REPLY_OPERATION, networkConfiguration._macAddress, destinationMac, networkConfiguration._ipAddress, destinationIpAddress);
    return &txBuffer;
}

struct EthernetBuffer* BuildArpRequest(unsigned char* destinationIpAddress)
{
    unsigned char broadcastMac[MAC_ADDRESS_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char ffBytesNumber = GetNetmaskFFBytesNumber(networkConfiguration._netmask);
    unsigned char useGatewayAsDestination = 0;
    for(unsigned char counter = 0; counter < ffBytesNumber; counter ++)
    {
        if(networkConfiguration._ipAddress[counter] != destinationIpAddress[counter])
        {
            useGatewayAsDestination = 1;
            break;
        }
    }

    BuildArpPacketImpl(ARP_REQUEST_OPERATION, networkConfiguration._macAddress, broadcastMac, networkConfiguration._ipAddress, useGatewayAsDestination ? networkConfiguration._gateway : destinationIpAddress);
    return &txBuffer;
}
