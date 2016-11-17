#include "arpCache.h"
#include "networkUtils.h"

extern ArpCache* M3ArpCachePtr = &M3ArpCache;

void CopyEntry(unsigned char index, struct ArpEntry* entry, uint32_t timestamp);
void UpdateEntryImpl(struct ArpEntry* entry, uint32_t timestamp);

void CopyEntry(unsigned char index, struct ArpEntry* entry, uint32_t timestamp)
{
	M3ArpCache._entries[index]._entryCreationTimestamp = timestamp;
    memcpy(M3ArpCache._entries[index]._ipAddress, &entry->_ipAddress[0], IPV4_LENGTH);
    memcpy(M3ArpCache._entries[index]._macAddress, &entry->_macAddress[0], MAC_ADDRESS_LENGTH);
    M3ArpCache._entries[index]._isEntryInited = 1;
    //printf("Entry ip: %d.%d.%d.%d received \r\n", srcEntry->_ipAddress[0], srcEntry->_ipAddress[1], srcEntry->_ipAddress[2], srcEntry->_ipAddress[3]);
    printf("M3 cache copy ip: %d.%d.%d.%d received \r\n", M3ArpCache._entries[index]._ipAddress[0], M3ArpCache._entries[index]._ipAddress[1],
    		M3ArpCache._entries[index]._ipAddress[2], M3ArpCache._entries[index]._ipAddress[3]);
}

void UpdateEntryImpl(struct ArpEntry* entry, uint32_t timestamp)
{
    //struct ArpCache cache = M3ArpCache;
	//struct ArpEntry* entry = &M3ArpCache._entries[index];
    //M3ArpCache._entries[index].
    entry->_entryCreationTimestamp = timestamp;
}

struct ArpEntry** GetEntriesForUpdate(uint32_t timestamp, unsigned char* entriesLength)
{
	struct ArpCache cache = M3ArpCache;
    *entriesLength = 0;
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter++)
    {
        if(M3ArpCache._entries[entryCounter]._isEntryInited && M3ArpCache._arpCacheUpdateTime + M3ArpCache._entries[entryCounter]._entryCreationTimestamp < timestamp)
            (*entriesLength)++;
    }

    if(*entriesLength == 0)
        return 0;

    struct ArpEntry* expiredEntries[*entriesLength];
    unsigned char expiredEntriesCounter = 0;
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter++)
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

    char checkResult = CheckEntryIsPresent(entry->_ipAddress);
    char fail = -1;
    if(checkResult != fail)
    {
        UpdateEntry(entry, timestamp);
        return;
    }

    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter++)
    {
        if(!M3ArpCache._entries[entryCounter]._isEntryInited)
        {
            CopyEntry(index, entry, timestamp);
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

char CheckEntryIsPresent(unsigned char* ipAddress)
{
    for(unsigned char index = 0; index < ARP_CACHE_SIZE; index++)
    {
        if(M3ArpCache._entries[index]._isEntryInited && CheckIpAddressEquals(M3ArpCache._entries[index]._ipAddress, ipAddress))
           return index;
    }
    return -1;
}

void UpdateEntry(struct ArpEntry* entry, uint32_t timestamp)
{
   char fail = -1;
   char index = CheckEntryIsPresent(entry->_ipAddress);
   if(index != fail)
       UpdateEntryImpl(&M3ArpCache._entries[index], timestamp);
}
