#include "ip.h"

/*unsigned short GetIpHeaderSize(struct EthernetBuffer* buffer)
{
    return (unsigned short)(buffer->_buffer[ETHERNET_PAYLOAD_INDEX] & 0x0F);
}*/

unsigned short GetIpFrameSize(struct EthernetBuffer* buffer)
{
    return (unsigned short)(buffer->_buffer[IP_PACKET_SIZE_INDEX] << 8) + (buffer->_buffer[IP_PACKET_SIZE_INDEX + 1]);
}

unsigned short GetChecksumForNonTcpPackets(void *address, unsigned short count)
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
