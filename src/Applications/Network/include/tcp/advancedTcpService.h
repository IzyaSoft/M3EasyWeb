#ifndef ADVANCEDTCPSERVICE_H
#define ADVANCEDTCPSERVICE_H

#include "tcp.h"
#include "networkService.h"
#include "networkApplicationConfig.h"

void HandleTcpServiceClockTick();
void ProcessTcpPacket(struct EthernetBuffer* rxBuffer);
//void SendTcpData(struct TcpHeader header, unsigned char* data, unsigned short dataLength);
//void SendTcpData(struct NetworkApplicationConfig* application, struct EthernetBuffer* txBuffer, unsigned short tcpDataLength, unsigned char lastPacket);
void SendTcpData(struct EthernetBuffer* txBuffer);
#endif
