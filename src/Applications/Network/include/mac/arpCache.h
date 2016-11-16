#ifndef ARPCACHE_H
#define ARPCACHE_H

#include "ArpEntry.h"

#define ARP_CACHE_SIZE 4

struct ArpCache
{
    uint32_t _arpCacheUpdateTime;
    struct ArpEntry _entries[ARP_CACHE_SIZE];
}M3ArpCache;

struct ArpEntry** CheckEntriesForUpdate(uint32_t timestamp, unsigned char* entriesLength);
void AddEntry(struct ArpEntry* entry, uint32_t timestamp);

#endif
