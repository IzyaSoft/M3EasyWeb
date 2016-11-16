#include "arpCache.h"

static void CopyEntry(unsigned char index, struct ArpEntry* entry, uint32_t timestamp)
{
    M3ArpCache._entries[index]._entryCreationTimestamp = timestamp;
    memcpy(&M3ArpCache._entries[index]._ipAddress, entry->_ipAddress, IPV4_LENGTH);
    memcpy(&M3ArpCache._entries[index]._macAddress, entry->_macAddress, MAC_ADDRESS_LENGTH);
}

struct ArpEntry** CheckEntriesForUpdate(uint32_t timestamp, unsigned char* entriesLength)
{
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter)
    {
        if(M3ArpCache._arpCacheUpdateTime + M3ArpCache._entries[entryCounter]._entryCreationTimestamp < timestamp)
            (*entriesLength)++;
    }

    struct ArpEntry* expiredEntries[*entriesLength];
    unsigned char expiredEntriesCounter = 0;
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter)
    {
        if(M3ArpCache._arpCacheUpdateTime + M3ArpCache._entries[entryCounter]._entryCreationTimestamp < timestamp)
        {
            expiredEntries[expiredEntriesCounter] = &M3ArpCache._entries[entryCounter];
            expiredEntriesCounter++;
        }
    }

    return expiredEntries;
}

void AddEntry(struct ArpEntry* entry, uint32_t timestamp)
{
    unsigned char index;
    uint32_t lastUpdatedEntryTimestamp = 0;
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter)
    {
        if(!M3ArpCache._entries[entryCounter]._isEntryInited)
        {
            CopyEntry(entryCounter, entry, timestamp);
            break;
        }
        if(lastUpdatedEntryTimestamp < M3ArpCache._entries[entryCounter]._entryCreationTimestamp)
        {
            index = entryCounter;
            lastUpdatedEntryTimestamp = M3ArpCache._entries[entryCounter]._entryCreationTimestamp;
        }
    }
    CopyEntry(index, entry, timestamp);
}
