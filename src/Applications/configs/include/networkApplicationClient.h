#ifndef NETWORKAPPLICATIONCLIENT_H
#define NETWORKAPPLICATIONCLIENT_H

#include <stdint.h>
#include "ethernet.h"
#include "tcpHandshakeInfo.h"

enum SocketStatus
{
    SOCKET_CLOSED = 0,
    SOCKET_OPENED = 1,
    SOCKET_CONNECTED = 2,
    SOCKET_CLOSING = 3,
    SOCKET_ACTIVE = 4
};

struct NetworkApplicationClient
{
    unsigned char _ipAddress[IPV4_LENGTH];
    unsigned char _macAddress[MAC_ADDRESS_LENGTH];
    unsigned short _tcpPort;
    unsigned char _retryCounter;
    unsigned char _socketStatus;
    unsigned char _tcpState;
    unsigned char _hasData;
    unsigned short _dataLength;
    struct TcpHandshakeInfo _handshakeInfo;
};

#endif
