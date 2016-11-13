#ifndef NETWORKAPPLICATIONCONFIG_H
#define NETWORKAPPLICATIONCONFIG_H

#include <stdint.h>
#include "networkApplicationClient.h"

struct NetworkApplicationConfig
{
    unsigned char _isTcpApplication; // otherwise UDP
    unsigned short _applicationPort;
    unsigned char _tcpState;
    void (*_applicationHandler) (struct EthernetBuffer*, struct TcpHeader* tcpHeader);
    struct NetworkApplicationClient _client;   // temporary is one, in future could be many
    uint32_t _sequenceNumber;
    uint32_t _acknowledgementNumber;
    uint32_t _notAcknowledgedNumber;
};

#endif
