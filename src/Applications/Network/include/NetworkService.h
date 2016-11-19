#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include "hal.h"
#include "ethernetBuffer.h"
#include "ethernet.h"
#include "tcp.h"

#define SMALL_BUFEER     72

extern unsigned char ethernetBuffer[MAX_ETH_FRAME_SIZE];
extern unsigned char localBuffer[SMALL_FRAME_SIZE];

void InitializeNetwork(struct EthernetConfiguration* ethernetConfiguration);
void HandleNetworkEvents();
unsigned char CheckIsPacketBrodcast(struct EthernetBuffer* buffer);
void HandleBrodcastPacket(struct EthernetBuffer* buffer);
void HandleIndividualAddressPacket(struct EthernetBuffer* buffer);
void HandleNetworkServiceClockTick(uint32_t clockValue);
void TransmitData(struct EthernetBuffer* buffer);
struct EthernetBuffer* GetEthernetBuffer();

#endif

