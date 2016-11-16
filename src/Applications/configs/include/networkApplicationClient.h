#ifndef NETWORKAPPLICATIONCLIENT_H
#define NETWORKAPPLICATIONCLIENT_H

#include <stdint.h>
#include "ethernet.h"

struct NetworkApplicationClient
{
    unsigned char _ipAddress[IPV4_LENGTH];
    unsigned char _macAddress[MAC_ADDRESS_LENGTH];
};

#endif
