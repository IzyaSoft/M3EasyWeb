#include "icmp.h"
#include "networkUtils.h"

void BuildIcmpPacket(struct EthernetBuffer* buffer)
{
    if(buffer->_buffer[ICMP_TYPE_INDEX] == ICMP_ECHO_REQUEST)
    {
        unsigned char* destinationMac[MAC_ADDRESS_LENGTH];
        unsigned char* destinationIpAddress[IPV4_LENGTH];
        unsigned char* sourceMac[MAC_ADDRESS_LENGTH];
        unsigned char* sourceIpAddress[IPV4_LENGTH];

        unsigned short ipFrameLength = GetIpFrameSize(buffer);
        unsigned short icmpDataCount = ipFrameLength - IP_HEADER_SIZE - ICMP_HEADER_SIZE;

        memcpy(destinationMac, &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
        memcpy(sourceMac, &buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
        memcpy(destinationIpAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
        memcpy(sourceIpAddress, &buffer->_buffer[IP_PACKET_HEADER_DESTINATION_IP_INDEX], IPV4_LENGTH);

        InsertEthernetHeader(buffer, sourceMac, destinationMac, IP_ETHERTYPE);
        InsertIpHeader(buffer, IPV4_VERSION, IP_HEADER_SIZE + ICMP_HEADER_SIZE + icmpDataCount, 0, 0, (TTL << 8) | ICMP_PROTOCOL, sourceIpAddress, destinationIpAddress);
        // ICMP
        SetWord(ICMP_ECHO_RESPONSE << 8, buffer, ICMP_TYPE_INDEX);
        SetWord(0, buffer, ICMP_CHEKSUM_INDEX);
        SetWord(SWAPBYTES(GetIpChecksum(&buffer->_buffer[IP_PACKET_DATA_INDEX], icmpDataCount + ICMP_HEADER_SIZE)), buffer, ICMP_CHEKSUM_INDEX);
        buffer->_storedBytes = ETHERNET_HEADER_SIZE + IP_HEADER_SIZE + ICMP_HEADER_SIZE + icmpDataCount;
    }
}
