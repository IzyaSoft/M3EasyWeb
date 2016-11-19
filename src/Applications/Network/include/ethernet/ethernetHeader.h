#ifndef ETHERNET_HEADER_H
#define ETHERNET_HEADER_H

#include <stdint.h>

#define HARDWARE_ADDRESS_LENGTH    6  // == MAC_ADDRESS_LENGTH

struct EthernetHeader
{
    unsigned char _sourceMac[HARDWARE_ADDRESS_LENGTH];
    unsigned char _destinationMac[HARDWARE_ADDRESS_LENGTH];
    unsigned short _etherType;
};

#endif
