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

void SetWord(unsigned short word, struct EthernetBuffer* buffer, uint32_t position)
{
	buffer->_buffer[position + 1] = (unsigned char) word & 0x00FF;
    buffer->_buffer[position] = (unsigned char) ((word &0xFF00) >> 8);
}
