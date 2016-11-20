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
    unsigned char _tcpState;       // todo : umv: 4 remove
    void (*_applicationHandler) (struct EthernetBuffer*);
    struct NetworkApplicationClient _client[MAX_CLIENTS_NUMBER];
    unsigned char _tcpFlags;       // todo : umv: 4 remove
    unsigned char _retryCounter;   // todo : umv: 4 remove
    unsigned char _socketStatus;   // todo : umv: 4 remove
    unsigned char _isEnabled;
};

#endif
