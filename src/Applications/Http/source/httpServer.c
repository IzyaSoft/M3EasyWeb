#include "hal.h"
#include "tcp.h"
#include "tcpHeader.h"
#include "tcpService.h"
#include "httpServer.h"

#include "demopage.h"
#include "demoheader.h"

unsigned int pagecounter = 100;
unsigned int adcValue = 0;

struct NetworkApplicationConfig* httpServerConfig;

void OpenServer(struct NetworkApplicationConfig* config)
{
    httpServerConfig = config;
    httpServerConfig->_isEnabled = 1;
    for(unsigned char clientCounter = 0; clientCounter < MAX_CLIENTS_NUMBER; clientCounter++)
        httpServerConfig->_client[clientCounter]._tcpState = LISTENING;
    //remove lines below
    if(config->_tcpState == CLOSED)
    {
        config->_tcpFlags &= ~TCP_ACTIVE_OPEN;
        config->_tcpState = LISTENING;
        config->_socketStatus = SOCK_ACTIVE;
    }
    //httpServerConfig->_client._handshakeInfo._sequenceNumber = 0;
    //httpServerConfig->_client._handshakeInfo._acknowledgementNumber = 0;
    //httpServerConfig->_client._handshakeInfo._unAcknowledgedSequenceNumber = 0;
}

void StartProcessing(struct EthernetBuffer* packedHttp)
{
    //if(httpServerConfig->_socketStatus & SOCK_DATA_AVAILABLE)
    //{
        //printf("HTTP Request received!\r\n");
        httpServerConfig->_socketStatus &= ~SOCK_DATA_AVAILABLE;
        if(httpServerConfig->_client[0]._hasData)
        		//_socketStatus & SOCK_TX_BUF_RELEASED)     // todo: umv create 1 flag to application
        {
            // 1. HTTP data parsing ...
            // 2. Find Handler ....
            // 3. Return result ....
            static unsigned char firstPartSent = 0;
            unsigned short demoPageLength = sizeof(demoPage) - 1;
            unsigned short demoHeaderLength = sizeof(demoResponseHeader) - 1;
            static struct EthernetBuffer txBuffer;
            static unsigned char txBufferStorage[1536];
            txBuffer._buffer = txBufferStorage;
            txBuffer._bufferCapacity = 1536;
            static unsigned char responseData[MAX_TCP_TX_DATA_SIZE];

            if(!firstPartSent)
            {
            memcpy(responseData, demoResponseHeader, demoHeaderLength);
            memcpy(&responseData[demoHeaderLength], demoPage, MAX_TCP_TX_DATA_SIZE - demoHeaderLength);
            InsertDynamicValues(responseData,  MAX_TCP_TX_DATA_SIZE);
            BuildTcpDataFrame(&txBuffer, httpServerConfig->_applicationPort, &httpServerConfig->_client[0], responseData, MAX_TCP_TX_DATA_SIZE);
            SendTcpData(httpServerConfig, &txBuffer, MAX_TCP_TX_DATA_SIZE);
            firstPartSent = 1;
            return;
            }
            else
            {
            unsigned short remainBytes = demoPageLength - MAX_TCP_TX_DATA_SIZE;

            memcpy(responseData, &demoPage[MAX_TCP_TX_DATA_SIZE], remainBytes);
            InsertDynamicValues(responseData, remainBytes);
            BuildTcpDataFrame(&txBuffer, httpServerConfig->_applicationPort, &httpServerConfig->_client[0], responseData, remainBytes);
            SendTcpData(httpServerConfig, &txBuffer, remainBytes);
            firstPartSent = 0;
            httpServerConfig->_client[0]._hasData = 0;
            return;
            }
        }
    //}
}

void InsertDynamicValues(unsigned char* buffer, unsigned short length)
{
    unsigned char *Key;
    char NewKey[5];
    unsigned int i;

    if (length < 4) return;                     // there can't be any special string

    Key = buffer;

    for (i = 0; i < (length - 3); i++)
    {
        if (*Key == 'A')
            if (*(Key + 1) == 'D')
                if (*(Key + 3) == '%')
                switch (*(Key + 2))
                {
                    case '8' :                                 // "AD8%"?
                    {
                        adcValue = GetAdcValue(5);
                        sprintf(NewKey, "0x%03X", adcValue);       // insert AD converter value
                        memcpy(Key, NewKey, 5);
                        break;
                    }
                    case '7' :                                 // "AD7%"?
                    {
                        sprintf(NewKey, "%3u", (adcValue*100)/4024);     // copy saved value from previous read
                        memcpy(Key, NewKey, 3);
                        break;
                    }
                    case '1' :                                 // "AD1%"?
                    {
                        sprintf(NewKey, "%3u", ++pagecounter);    // increment and insert page counter
                        memcpy(Key, NewKey, 3);
                        *(Key + 3) = ' ';
                        break;
                    }
                }
        Key++;
    }
}
