#ifndef ETHERNET_HEADER_H
#define ETHERNET_HEADER_H

#include <stdint.h>
#include "ethernet.h"

struct EthernetHeader
{
    unsigned char _sourceMac[MAC_ADDRESS_LENGTH];
    unsigned char _destinationMac[MAC_ADDRESS_LENGTH];
    unsigned short _etherType;
};

#endif
