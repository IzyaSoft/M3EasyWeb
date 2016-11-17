#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include "hal.h"
#include "ethernetBuffer.h"
#include "ethernet.h"
#include "tcp.h"

//#define MAX_LOCAL_BUFFER_SIZE_LIMIT                   127

extern unsigned char ethernetBuffer[MAX_ETH_FRAME_SIZE];

void InitializeNetwork(struct EthernetConfiguration* ethernetConfiguration);
void HandleNetworkEvents();
unsigned char CheckIsPacketBrodcast(struct EthernetBuffer* buffer);
void HandleBrodcastPacket(struct EthernetBuffer* buffer);
void HandleIndividualAddressPacket(struct EthernetBuffer* buffer);
void HandleNetworkServiceClockTick(uint32_t clockValue);
void TransmitData(struct EthernetBuffer* buffer);
unsigned char HasApplicationData();   // todo: umv: to do return as signed if non successfully -1 and app number if successfully....
struct EthernetBuffer* GetEthernetBuffer();

#define IP_VER_IHL           0x4500              // IPv4, Header Length = 5x32 bit

#define IP_FLAG_DONTFRAG     0x4000              // don't fragment IP frame
#define IP_FLAG_MOREFRAG     0x2000              // more fragments available
#define IP_FRAGOFS_MASK      0x1FFF              // indicates where this fragment belongs

#define DATA_OFS_MASK        0xF000              // number of 32 bit words in the TCP Header

// define some TCP standard-ports, useful for testing...
#define TCP_PORT_ECHO        7                   // echo
#define TCP_PORT_DISCARD     9                   // discard
#define TCP_PORT_DAYTIME     13                  // daytime
#define TCP_PORT_QOTD        17                  // quote of the day
#define TCP_PORT_CHARGEN     19                  // character generator
#define TCP_PORT_HTTP        80                  // word wide web HTTP

#endif

