#ifndef NETWORKAPPLICATIONCLIENT_H
#define NETWORKAPPLICATIONCLIENT_H

#include <stdint.h>
#include "ethernet.h"
#include "tcpHandshakeInfo.h"

struct NetworkApplicationClient
{
    unsigned char _ipAddress[IPV4_LENGTH];
    unsigned char _macAddress[MAC_ADDRESS_LENGTH];
    unsigned short _tcpPort;
    unsigned char _retryCounter;
    unsigned char _socketStatus;
    unsigned char _tcpState;
    struct TcpHandshakeInfo _handshakeInfo;
};

#endif
