#include <stdio.h>
#include "arp.h"
#include "arpCache.h"
#include "ip.h"
#include "networkService.h"
#include "networkConfiguration.h"
#include "networkUtils.h"
#include "debugPrintFunctions.h"


extern struct NetworkConfiguration networkConfiguration;
unsigned char hasData;
static struct EthernetBuffer rxBuffer;
uint32_t tickValue = 0;
extern struct ArpCache* M3ArpCachePtr;

static void WriteData(struct EthernetBuffer* txBuffer, unsigned char debugPrintEnabled)
{
    Write(txBuffer);
    if(debugPrintEnabled)
    {
        printf("Outgoing Packet: ");
        printStringHexSymbols(txBuffer->_buffer, txBuffer->_storedBytes, 6);
	}

}

static void ReadData(struct EthernetBuffer* rxBuffer, unsigned char debugPrintEnabled)
{
    Read(rxBuffer);
    if(debugPrintEnabled)
    {
        printf("Incoming Packet: ");
        printStringHexSymbols(rxBuffer->_buffer, rxBuffer->_storedBytes, 6);
    }
}

void SendArpRequest(unsigned char* ipAddress)
{
    struct EthernetBuffer arpRequestBuffer;
    unsigned char arpBuffer[60];
    arpRequestBuffer._buffer = arpBuffer;
    arpRequestBuffer._bufferCapacity = 60;
    BuildArpRequest(&arpRequestBuffer, ipAddress);
    WriteData(&arpRequestBuffer, 0);
}

void TransmitData(struct EthernetBuffer* buffer)
{
	WriteData(buffer, 0);
}

void InitializeNetwork(struct EthernetConfiguration* ethernetConfiguration)
{
    InitializeEthrernet(ethernetConfiguration);
    InitArpCache(50);
    //M3ArpCache._arpCacheUpdateTime = 50;
}

// Network Packet Routing
void HandleNetworkEvents()
{
    if(CheckIsDataAvailable())
    {
        uint32_t dataSize = CheckAvailableDataSize();
        // unsigned char localBuffer[MAX_LOCAL_BUFFER_SIZE_LIMIT] = {};  // todo: umv possibly do malloc + free
        // struct EthernetBuffer rxBuffer;
        rxBuffer._buffer = ethernetBuffer;
        rxBuffer._bufferCapacity = MAX_ETH_FRAME_SIZE;
        rxBuffer._storedBytes = 0;
/*        if(dataSize <= MAX_LOCAL_BUFFER_SIZE_LIMIT)
        {
            // local buffer usage
            rxBuffer._buffer = localBuffer;
            rxBuffer._bufferCapacity = MAX_LOCAL_BUFFER_SIZE_LIMIT;
        }
        else
        {
            rxBuffer._buffer = ethernetBuffer;
            rxBuffer._bufferCapacity = MAX_ETH_FRAME_SIZE;
        }*/

        ReadData(&rxBuffer, 0);
        if(CheckIsPacketBrodcast(&rxBuffer))
        {
            HandleBrodcastPacket(&rxBuffer);
        }
        else
        {
            HandleIndividualAddressPacket(&rxBuffer);
        }

    }
}

unsigned char CheckIsPacketBrodcast(struct EthernetBuffer* buffer)
{
    for(unsigned char counter = 0; counter < MAC_ADDRESS_LENGTH; counter++)
        if(buffer->_buffer[counter] != 0xFF)
            return 0;
    return 1;
}

void HandleBrodcastPacket(struct EthernetBuffer* buffer)
{
    unsigned short etherType = GetEtherType(buffer);
    if(etherType == ARP_ETHERTYPE)
    {
        BuildArpReply(buffer);
        WriteData(buffer, 0);
    }
}

void HandleIndividualAddressPacket(struct EthernetBuffer* buffer)
{
    unsigned short etherType = GetEtherType(buffer);
    if(etherType == ARP_ETHERTYPE)
    {
        if(GetWord(buffer, ARP_OPCODE_INDEX) == ARP_REPLY_OPERATION)
        {
            struct ArpEntry entry;
            memcpy(entry._macAddress, &buffer->_buffer[ARP_SENDER_MAC_INDEX], MAC_ADDRESS_LENGTH);
            memcpy(entry._ipAddress, &buffer->_buffer[ARP_SENDER_IP_INDEX], IPV4_LENGTH);
            //entry._isEntryInited = 1;
            //entry._entryCreationTimestamp = tickValue;

            //printf("Arp reply for ip: %d.%d.%d.%d received \r\n", buffer->_buffer[ARP_SENDER_IP_INDEX],buffer->_buffer[ARP_SENDER_IP_INDEX + 1],
            //buffer->_buffer[ARP_SENDER_IP_INDEX + 2], buffer->_buffer[ARP_SENDER_IP_INDEX + 3]);
            //printf("Arp reply ip copy: %d.%d.%d.%d received \r\n", entry._ipAddress[0],entry._ipAddress[1], entry._ipAddress[2], entry._ipAddress[3]);
            AddEntry(&entry, tickValue);
        }
    }

    else if(etherType == IP_ETHERTYPE)
    {
    	static unsigned char arpSend = 0;
        if((GetWord(buffer, ETHERNET_PAYLOAD_INDEX) & 0xFF00) == IPV4_VERSION)
        {
            if(! (GetWord(buffer, IP_PACKET_FLAGS_INDEX) & (IP_FLAG_MOREFRAG | IP_FRAGOFS_MASK)))
            {
                switch(buffer->_buffer[IP_PACKET_PROTOCOL_INDEX])
                {
                    case ICMP_PROTOCOL:
                         BuildIcmpPacket(buffer);
                         Write(buffer);
                         //todo: umv: remove in production
                         if(!arpSend)
                         {
                             unsigned char ip1[] = {192, 168, 200, 10};
                             SendArpRequest(ip1);
                             unsigned char ip2[] = {192, 168, 100, 185};
                             SendArpRequest(ip2);
                             arpSend = 1;
                         }
                         break;
                    case TCP_PROTOCOL:
                         hasData = HandleTcpPacket(buffer);
                         break;
                    case UDP_PROTOCOL:
                         break;
                }
            }
        }
    }
}

/*unsigned char HasApplicationData()
{
    if(hasData)
    {
        hasData = 0;
        return 1;
    }
    return 0;
}*/

struct EthernetBuffer* GetEthernetBuffer()
{
    return &rxBuffer;
}

void HandleNetworkServiceClockTick(uint32_t clockValue)
{
    tickValue++;
    for(unsigned char entryCounter = 0; entryCounter < ARP_CACHE_SIZE; entryCounter++)
    {
        if(M3ArpCachePtr->_entries[entryCounter]._isEntryInited &&
           M3ArpCachePtr->_arpCacheUpdateTime + M3ArpCachePtr->_entries[entryCounter]._entryCreationTimestamp < tickValue)
        {
            //printf("Sending arp update query for %d.%d.%d.%d ip \r\n", M3ArpCachePtr->_entries[entryCounter]._ipAddress[0], M3ArpCachePtr->_entries[entryCounter]._ipAddress[1],
                     //M3ArpCachePtr->_entries[entryCounter]._ipAddress[2], M3ArpCachePtr->_entries[entryCounter]._ipAddress[3]);
            SendArpRequest(M3ArpCachePtr->_entries[entryCounter]._ipAddress);
        }
    }
}
