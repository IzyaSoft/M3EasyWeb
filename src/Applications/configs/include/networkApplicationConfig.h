#ifndef NETWORKAPPLICATIONCONFIG_H
#define NETWORKAPPLICATIONCONFIG_H

#include <stdint.h>
#include "networkApplicationClient.h"
#include "tcpServiceContext.h"

struct NetworkApplicationConfig
{
    unsigned char _isTcpApplication;            // otherwise UDP
    unsigned short _applicationPort;
    unsigned char _tcpState;
    void (*_applicationHandler) (struct EthernetBuffer*, struct TcpHeader* tcpHeader);
    struct NetworkApplicationClient _client;   // temporarily is one, in future could be many
    struct TcpServiceContext _context;
    unsigned char _tcpFlags;                   // make sense when _isTcpApplication is 1
    unsigned char _retryCounter;
    unsigned char _socketStatus;
};

#endif
