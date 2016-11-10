#include "icmp.h"
#include "networkUtils.h"

void BuildIcmpPacket(struct EthernetBuffer* buffer)
{
    if(buffer->_buffer[ICMP_TYPE_INDEX] == ICMP_ECHO_REQUEST)
    {
        unsigned char* destinationMac[MAC_ADDRESS_LENGTH];
        unsigned char* destinatioIpAddress[IPV4_LENGTH];
        unsigned char* sourceMac[MAC_ADDRESS_LENGTH];
        unsigned char* sourceIpAddress[IPV4_LENGTH];

        unsigned short ipFrameLength = GetIpFrameSize(buffer);
        unsigned short icmpDataCount = ipFrameLength - IP_HEADER_SIZE - ICMP_HEADER_SIZE;

        memcpy(destinationMac, &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
        memcpy(sourceMac, &buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
        memcpy(destinatioIpAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
        memcpy(sourceIpAddress, &buffer->_buffer[IP_PACKET_HEADER_DESTINATION_IP_INDEX], IPV4_LENGTH);
        // Ethernet
        memcpy(&buffer->_buffer[ETHERNET_DESTINATION_ADDRESS_INDEX], destinationMac, MAC_ADDRESS_LENGTH);
        memcpy(&buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], sourceMac, MAC_ADDRESS_LENGTH);
        *(unsigned short *)&buffer->_buffer[ETHERNET_ETHERTYPE_INDEX] = SWAPBYTES(IP_ETHERTYPE);
        // IP
        *(unsigned short *)&buffer->_buffer[ETHERNET_PAYLOAD_INDEX] = SWAPBYTES(IPV4_VERSION);
        WriteWordAsBigEndian(&buffer->_buffer[IP_PACKET_SIZE_INDEX], IP_HEADER_SIZE + ICMP_HEADER_SIZE + icmpDataCount);
        *(unsigned short *)&buffer->_buffer[ IP_PACKET_ID_INDEX] = 0;
        *(unsigned short *)&buffer->_buffer[IP_PACKET_FLAGS_INDEX] = 0;
        *(unsigned short *)&buffer->_buffer[IP_PACKET_TTL_INDEX] = SWAPBYTES((TTL << 8) | ICMP_PROTOCOL);
        *(unsigned short *)&buffer->_buffer[IP_PACKET_HEADER_CHECKSUM_INDEX] = 0;
        memcpy(&buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], sourceIpAddress, IPV4_LENGTH);
        memcpy(&buffer->_buffer[IP_PACKET_HEADER_DESTINATION_IP_INDEX], destinatioIpAddress, IPV4_LENGTH);
        *(unsigned short *)&buffer->_buffer[IP_PACKET_HEADER_CHECKSUM_INDEX] = GetChecksumForNonTcpPackets(&buffer->_buffer[ETHERNET_PAYLOAD_INDEX], IP_HEADER_SIZE);
        // ICMP
        *(unsigned short *)&buffer->_buffer[ICMP_TYPE_INDEX] = SWAPBYTES(ICMP_ECHO_RESPONSE << 8);
        *(unsigned short *)&buffer->_buffer[ICMP_CHEKSUM_INDEX] = 0;

        // CopyFromFrame_EMAC(&buffer->_buffer[ICMP_DATA_OFS], ICMPDataCount);
        *(unsigned short *)&buffer->_buffer[ICMP_CHEKSUM_INDEX] = GetChecksumForNonTcpPackets(&buffer->_buffer[IP_PACKET_DATA_INDEX], icmpDataCount + ICMP_HEADER_SIZE);

        buffer->_storedBytes = ETHERNET_HEADER_SIZE + IP_HEADER_SIZE + ICMP_HEADER_SIZE + icmpDataCount;
    }

        /*

        // ICMP
        *(unsigned short *)&TxFrame2[ICMP_TYPE_CODE_OFS] = SWAPB(ICMP_ECHO_REPLY << 8);
        *(unsigned short *)&TxFrame2[ICMP_CHKSUM_OFS] = 0;                   // initialize checksum field

        CopyFromFrame_EMAC(&TxFrame2[ICMP_DATA_OFS], ICMPDataCount);        // get data to echo...
        *(unsigned short *)&TxFrame2[ICMP_CHKSUM_OFS] = CalcChecksum(&TxFrame2[IP_DATA_OFS], ICMPDataCount + ICMP_HEADER_SIZE, 0);

        TxFrame2Size = ETH_HEADER_SIZE + IP_HEADER_SIZE + ICMP_HEADER_SIZE + ICMPDataCount;*/
}
