#ifndef ETHERNET_BUFFER_H
#define ETHERNET_BUFFER_H

#include <stdint.h>

struct EthernetBuffer
{
    uint32_t* _buffer;
    uint32_t _bufferCapacity;
    uint32_t _storedBytes;
};

#endif
