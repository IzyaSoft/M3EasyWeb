#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <stdint.h>
#include "networkApplicationConfig.h"
#include "ethernetBuffer.h"
#include "tcpHeader.h"

void OpenServer(struct NetworkApplicationConfig* config, unsigned char activeOpen);
void StartProcessing(struct EthernetBuffer* packedHttp);

#endif
