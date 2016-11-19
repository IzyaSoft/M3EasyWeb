#include "ip.h"
#include "networkUtils.h"
#include <string.h>
unsigned short GetIpFrameSize(struct EthernetBuffer* buffer)
{
    return (unsigned short)(buffer->_buffer[IP_PACKET_SIZE_INDEX] << 8) + (buffer->_buffer[IP_PACKET_SIZE_INDEX + 1]);
}

unsigned short GetIpChecksum(void *address, unsigned short count)
{
    unsigned long checkSum = 0;
    unsigned short * addressShort = address;

    while (count > 1)
    {
        checkSum += *addressShort++;
        count -= 2;
    }

    if(count)                                     // add left-over byte, if any
        checkSum += *(unsigned char*)addressShort;

    while (checkSum >> 16)                        // fold 32-bit sum to 16 bits
        checkSum = (checkSum & 0xFFFF) + (checkSum >> 16);

    return ~checkSum;
}

void InsertIpHeader(struct EthernetBuffer* buffer, unsigned short ipVersion, unsigned short packetSize, unsigned short packetId, unsigned short packetFlags,
                    unsigned short ttl, unsigned char* sourceIpAddress, unsigned char* destinationIpAddress)
{
    SetWord(ipVersion, buffer, ETHERNET_PAYLOAD_INDEX);
    SetWord(packetSize, buffer, IP_PACKET_SIZE_INDEX);
    SetWord(packetId, buffer, IP_PACKET_ID_INDEX);
    SetWord(packetFlags, buffer, IP_PACKET_FLAGS_INDEX);
    SetWord(ttl, buffer, IP_PACKET_TTL_INDEX);
    SetWord(0, buffer, IP_PACKET_HEADER_CHECKSUM_INDEX);
    memcpy(&buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], sourceIpAddress, IPV4_LENGTH);
    memcpy(&buffer->_buffer[IP_PACKET_HEADER_DESTINATION_IP_INDEX], destinationIpAddress, IPV4_LENGTH);
    SetWord(htons(GetIpChecksum(&buffer->_buffer[ETHERNET_PAYLOAD_INDEX], IP_HEADER_SIZE)), buffer, IP_PACKET_HEADER_CHECKSUM_INDEX);
}
