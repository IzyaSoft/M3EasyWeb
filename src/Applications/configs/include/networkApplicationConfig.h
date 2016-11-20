#ifndef NETWORKAPPLICATIONCONFIG_H
#define NETWORKAPPLICATIONCONFIG_H

#include <stdint.h>
#include "networkApplicationClient.h"

#define MAX_CLIENTS_NUMBER               16
#define DEFAULT_RETRY_COUNTER            8

struct NetworkApplicationConfig
{
    unsigned char _isTcpApplication;            // otherwise UDP
    unsigned short _applicationPort;
    unsigned char _tcpState;
    void (*_applicationHandler) (struct EthernetBuffer*);
    struct NetworkApplicationClient _client[MAX_CLIENTS_NUMBER];
    unsigned char _tcpFlags;                   // make sense when _isTcpApplication is 1
    unsigned char _retryCounter;
    unsigned char _socketStatus;
};

#endif
