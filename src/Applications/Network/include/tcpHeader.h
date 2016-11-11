#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <stdint.h>

struct TcpHeader
{
    unsigned short _sourcePort;
    unsigned short _destinationPort;
    unsigned char _headerSize;
    unsigned char _flags;
    unsigned short _windowsSize;
    unsigned short _checkSum;
    unsigned short _urgency;
    uint32_t _sequenceNumber;
    uint32_t _acknoledgemengtNumber;
    uint32_t _dataIndex;
};

#endif
