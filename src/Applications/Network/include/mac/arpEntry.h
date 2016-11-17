#ifndef ARPENTRY_H
#define ARPENTRY_H

#include <stdint.h>
#include "ethernet.h"

struct ArpEntry
{
    unsigned char _ipAddress[IPV4_LENGTH];
    unsigned char _macAddress[MAC_ADDRESS_LENGTH];
    uint32_t _entryCreationTimestamp;
    unsigned char _isEntryInited;
};

#endif
