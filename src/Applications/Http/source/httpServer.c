#include "httpServer.h"
#include "tcpService.h"

struct NetworkApplicationConfig* httpServerConfig;

void OpenServer(struct NetworkApplicationConfig* config, unsigned char activeOpen)
{
    httpServerConfig = config;
    if(!activeOpen)
    {
        if(config->_tcpState == CLOSED)
        {
            config->_tcpFlags &= ~TCP_ACTIVE_OPEN;                // passive open!
            config->_tcpState = LISTENING;
            config->_socketStatus = SOCK_ACTIVE;                  // reset, socket now active
        }
    }

    else
    {
        if(config->_tcpState == CLOSED || config->_tcpState == LISTENING)
        {
            config->_tcpFlags |= TCP_ACTIVE_OPEN;                // passive open!
            //todo: umv: not fully ready!
            // config->_tcpState = LISTENING;
            // send arp
            config->_socketStatus = SOCK_ACTIVE;                  // reset, socket now active
        }
    }
}

void StartProcessing(struct EthernetBuffer* packedHttp)
{
    printf("HTTP Request received!\r\n");
}
