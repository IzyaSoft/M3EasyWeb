#ifndef ETHERNET_CONFIGURATION_H
#define ETHERNET_CONFIGURATION_H

#include <stdint.h>

enum EthernetSpeedMode
{
    M10 = 1,   // 10 Mbit
    M100 = 2,  // 100 Mbit
    G1 = 3     // 1 Gbit
};

enum EthernetChannelMode
{
    FullDuplex = 1,
    HalfDuplex = 2
};

struct EthernetConfiguration
{
    unsigned char* _macAddress;
    unsigned char _useAutoNegotiation;
    unsigned char _ethernetSpeedMode;
    unsigned char _ethernetChannelMode;
};

#endif
