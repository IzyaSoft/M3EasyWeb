#ifndef TCPSERVICE_H
#define TCPSERVICE_H
#include "networkService.h"

void HandleTcpPacket(struct EthernetBuffer* buffer);
void HandleTcpServiceClockTick();
#endif
