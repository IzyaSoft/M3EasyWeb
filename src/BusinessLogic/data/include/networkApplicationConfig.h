#ifndef NETWORKAPPLICATIONCONFIG_H
#define NETWORKAPPLICATIONCONFIG_H

#include <stdint.h>
#include "tcp.h"

struct NetworkApplicationConfig
{
    unsigned char _isTcpApplication; // otherwise UDP
    unsigned short _applicationPort;
    enum TcpState _tcpState;
    void (*applicationHandler) (struct EthernetBuffer*, struct TcpHeader* tcpHeader);
};

#endif
