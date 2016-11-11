#include "networkUtils.h"

void WriteWordAsBigEndian(unsigned char *address, unsigned short data)
{
    *address++ = data >> 8;
    *address = (char)data;
}

unsigned short GetWord(struct EthernetBuffer* buffer, uint32_t index)
{
    return ((unsigned short) buffer->_buffer[index] << 8) + buffer->_buffer[index + 1];
}

uint32_t GetDoubleWord(struct EthernetBuffer* buffer, uint32_t index)
{
    return ((uint32_t) buffer->_buffer[index] << 24) + ((uint32_t) buffer->_buffer[index + 1] << 16) +
           ((uint32_t) buffer->_buffer[index + 2] << 8) + buffer->_buffer[index + 3];
}

void SetWord(unsigned short word, struct EthernetBuffer* buffer, uint32_t position)
{
	buffer->_buffer[position + 1] = (unsigned char) word & 0x00FF;
    buffer->_buffer[position] = (unsigned char) ((word &0xFF00) >> 8);
}

static uint32_t Revert4BytesOrder(uint32_t value)
{
    uint32_t result = ((value & 0x000000FF) << 24) + ((value & 0x0000FF00) << 8) + ((value & 0x00FF0000) >> 8) + ((value & 0xFF000000) >> 24);
    return result;
}

static unsigned short Revert2BytesOrder(unsigned short  value)
{
	unsigned short  result = ((value & 0x00FF) << 8) + ((value & 0xFF00) >> 8);
    return result;
}

// host - little endian, network - big endian
uint32_t htonl(uint32_t value)
{
    return Revert4BytesOrder(value);

}

uint32_t ntohl(uint32_t value)
{
    return Revert4BytesOrder(value);
}

unsigned short htons(unsigned short value)
{
    return Revert2BytesOrder(value);
}
unsigned short ntohs(unsigned short value)
{
    return Revert2BytesOrder(value);
}
