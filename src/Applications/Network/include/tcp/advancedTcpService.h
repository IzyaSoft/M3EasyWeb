#ifndef ADVANCEDTCPSERVICE_H
#define ADVANCEDTCPSERVICE_H

#include "tcp.h"
#include "networkService.h"
#include "networkApplicationConfig.h"

void ProcessTcpPacket(struct EthernetBuffer* rxBuffer);
void SendTcpData(struct TcpHeader header, unsigned char* data, unsigned short dataLength);

#endif
