#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <stdint.h>

struct TcpHeader
{
    // TCP Header Fields
    unsigned short _sourcePort;
    unsigned short _destinationPort;
    unsigned char _headerSize;
    unsigned char _flags;
    unsigned short _windowsSize;
    unsigned short _checkSum;
    unsigned short _urgency;
    uint32_t _sequenceNumber;
    uint32_t _acknowledgementNumber;
    uint32_t _dataIndex;
    // IP header fields
    unsigned char _sourceIpAddress[IPV4_LENGTH];
    unsigned char _destinationIpAddress[IPV4_LENGTH];
    // Ethernet header fields
    unsigned char _sourceMacAddress[MAC_ADDRESS_LENGTH];
    unsigned char _destinationMacAddress[MAC_ADDRESS_LENGTH];
};

#endif
