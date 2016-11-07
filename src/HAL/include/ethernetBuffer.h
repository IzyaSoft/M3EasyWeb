#ifndef ETHERNET_BUFFER_H
#define ETHERNET_BUFFER_H

#include <stdint.h>

struct EthernetBuffer
{
    unsigned char* _buffer;
    uint32_t _bufferCapacity;
    uint32_t _storedBytes;
};

#endif
