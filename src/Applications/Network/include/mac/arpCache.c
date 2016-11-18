#include "arpCache.h"
#include "networkUtils.h"

extern struct ArpCache* M3ArpCachePtr = &M3ArpCache;

//void CopyEntry(unsigned char index, struct ArpEntry* entry, uint32_t timestamp);
void CopyEntry(struct ArpEntry* srcEntry, struct ArpEntry* dstEntry, uint32_t timestamp);
void UpdateEntryImpl(struct ArpEntry* entry, uint32_t timestamp, unsigned char* ipAddress, unsigned char* macAddress);
//struct ArpEntry* GetArpEntryPointer(unsigned char index);

/*void CopyEntry(unsigned char index, struct ArpEntry* entry, uint32_t timestamp)
{
    M3ArpCachePtr->_entries[index]._entryCreationTimestamp = timestamp;
    memcpy(M3ArpCachePtr->_entries[index]._ipAddress, &entry->_ipAddress[0], IPV4_LENGTH);
    memcpy(M3ArpCachePtr->_entries[index]._macAddress, &entry->_macAddress[0], MAC_ADDRESS_LENGTH);
    M3ArpCachePtr->_entries[index]._isEntryInited = 1;
    //printf("Entry ip: %d.%d.%d.%d received \r\n", srcEntry->_ipAddress[0], srcEntry->_ipAddress[1], srcEntry->_ipAddress[2], srcEntry->_ipAddress[3]);
    printf("M3 cache copy ip: %d.%d.%d.%d received \r\n", M3ArpCachePtr->_entries[index]._ipAddress[0], M3ArpCachePtr->_entries[index]._ipAddress[1],
    		M3ArpCachePtr->_entries[index]._ipAddress[2], M3ArpCachePtr->_entries[index]._ipAddress[3]);
    struct ArpCache* cache = M3ArpCachePtr;
}*/

void CopyEntry(struct ArpEntry* srcEntry, struct ArpEntry* dstEntry, uint32_t timestamp)
{
    dstEntry->_entryCreationTimestamp = timestamp;
    memcpy(&dstEntry->_ipAddress[0], &srcEntry->_ipAddress[0], IPV4_LENGTH);
    memcpy(&dstEntry->_macAddress[0], &srcEntry->_macAddress[0], MAC_ADDRESS_LENGTH);
    dstEntry->_isEntryInited = 1;
    //printf("Entry ip: %d.%d.%d.%d received \r\n", srcEntry->_ipAddress[0], srcEntry->_ipAddress[1], srcEntry->_ipAddress[2], srcEntry->_ipAddress[3]);
    //printf("M3 cache copy ip: %d.%d.%d.%d received \r\n", dstEntry->_ipAddress[0], dstEntry->_ipAddress[1], dstEntry->_ipAddress[2], dstEntry->_ipAddress[3]);
    struct ArpCache* cache = M3ArpCachePtr;
}

void UpdateEntryImpl(struct ArpEntry* entry, uint32_t timestamp, unsigned char* ipAddress, unsigned char* macAddress)
{
	struct ArpCache* cache = M3ArpCachePtr;
    entry->_entryCreationTimestamp = timestamp;
    memcpy(&entry->_ipAddress[0], ipAddress, IPV4_LENGTH);
    memcpy(&entry->_macAddress[0], macAddress, MAC_ADDRESS_LENGTH);
}

void InitArpCache(uint32_t updateTime)
{
    M3ArpCachePtr->_arpCacheUpdateTime = updateTime;
}

/*uint32_t* GetEntriesForUpdate(uint32_t timestamp, unsigned char* entriesLength)
{
	struct ArpCache* cache = M3ArpCachePtr;
    unsigned char expiredEntriesCount = 0;
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter++)
    {
        if(M3ArpCachePtr->_entries[entryCounter]._isEntryInited && M3ArpCachePtr->_arpCacheUpdateTime + M3ArpCachePtr->_entries[entryCounter]._entryCreationTimestamp < timestamp)
        	expiredEntriesCount++;
    }

    uint32_t* expiredEntries[expiredEntriesCount];
    for(unsigned char entryCounter = 0, expiredCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter++)
    {
        if(M3ArpCachePtr->_entries[entryCounter]._isEntryInited &&
           M3ArpCachePtr->_arpCacheUpdateTime + M3ArpCachePtr->_entries[entryCounter]._entryCreationTimestamp < timestamp)
        {
        	expiredEntries[expiredCounter] = &M3ArpCachePtr->_entries[entryCounter];
            expiredCounter++;
        }
    }

    * entriesLength = expiredEntriesCount;
    return &expiredEntries[0];
}*/

void AddEntry(struct ArpEntry* entry, uint32_t timestamp)
{
    unsigned char index = 0;
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
        if(!M3ArpCachePtr->_entries[entryCounter]._isEntryInited)
        {
            //CopyEntry(index, entry, timestamp);
            CopyEntry(entry, &M3ArpCachePtr->_entries[entryCounter], timestamp);
            return;
        }
        if(lastUpdatedEntryTimestamp < M3ArpCachePtr->_entries[entryCounter]._entryCreationTimestamp)
        {
            index = entryCounter;
            lastUpdatedEntryTimestamp = M3ArpCachePtr->_entries[entryCounter]._entryCreationTimestamp;
        }
    }
    CopyEntry(entry, &M3ArpCachePtr->_entries[index], timestamp);
    //CopyEntry(index, entry, timestamp);
}

char CheckEntryIsPresent(unsigned char* ipAddress)
{
    for(unsigned char index = 0; index < ARP_CACHE_SIZE; index++)
    {
        if(M3ArpCachePtr->_entries[index]._isEntryInited && CheckIpAddressEquals(M3ArpCachePtr->_entries[index]._ipAddress, ipAddress))
           return index;
    }
    return -1;
}

void UpdateEntry(struct ArpEntry* entry, uint32_t timestamp)
{
    char fail = -1;
    char index = CheckEntryIsPresent(entry->_ipAddress);
    if(index != fail)
        UpdateEntryImpl(&M3ArpCachePtr->_entries[index], timestamp, entry->_ipAddress, entry->_macAddress);
}
