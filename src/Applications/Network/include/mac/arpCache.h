#ifndef ARPCACHE_H
#define ARPCACHE_H

#include "ArpEntry.h"

#define ARP_CACHE_SIZE 4

struct ArpCache
{
    uint32_t _arpCacheUpdateTime;
    struct ArpEntry _entries[ARP_CACHE_SIZE];
};

struct ArpCache M3ArpCache;

void InitArpCache(uint32_t updateTime);
//uint32_t* GetEntriesForUpdate(uint32_t timestamp, unsigned char* entriesLength);
void AddEntry(struct ArpEntry* entry, uint32_t timestamp);
char CheckEntryIsPresent(unsigned char* ipAddress);
void UpdateEntry(struct ArpEntry* entry, uint32_t timestamp);

#endif
