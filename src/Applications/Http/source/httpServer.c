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
    if(config->_tcpState == CLOSED)
    {
        config->_tcpFlags &= ~TCP_ACTIVE_OPEN;                // passive open!
        config->_tcpState = LISTENING;
        config->_socketStatus = SOCK_ACTIVE;                  // reset, socket now active
    }
    httpServerConfig->_context._sequenceNumber = 0;
    httpServerConfig->_context._acknowledgementNumber = 0;
    httpServerConfig->_context._unAcknowledgedSequenceNumber = 0;
}

void StartProcessing(struct EthernetBuffer* packedHttp)
{
    if(httpServerConfig->_socketStatus & SOCK_DATA_AVAILABLE)
    {
        printf("HTTP Request received!\r\n");
        httpServerConfig->_socketStatus &= ~SOCK_DATA_AVAILABLE;
        if(httpServerConfig->_socketStatus & SOCK_TX_BUF_RELEASED)     // todo: umv create 1 flag to application
        {
            // 1. HTTP data parsing ...
            // 2. Find Handler ....
            // 3. Return result ....
            struct TcpHeader tcpHeader;
            ReadTcpHeader(packedHttp, &tcpHeader);
            unsigned short demoPageLength = sizeof(demoPage);
            unsigned short demoHeaderLength = sizeof(demoResponseHeader);
            static struct EthernetBuffer txBuffer;
            static unsigned char txBufferStorage[1536];
            txBuffer._buffer = txBufferStorage;
            txBuffer._bufferCapacity = 1536;
            static unsigned char responseData[MAX_TCP_TX_DATA_SIZE];

            memcpy(responseData, demoResponseHeader, demoHeaderLength);

            memcpy(&responseData[demoHeaderLength], demoPage, MAX_TCP_TX_DATA_SIZE - demoHeaderLength);
            InsertDynamicValues(responseData,  MAX_TCP_TX_DATA_SIZE);                   // exchange some strings...
            BuildTcpDataFrame(&tcpHeader, &txBuffer, httpServerConfig, responseData, MAX_TCP_TX_DATA_SIZE);
            SendTcpData(httpServerConfig, &txBuffer, MAX_TCP_TX_DATA_SIZE);

            unsigned short remainBytes = demoPageLength - MAX_TCP_TX_DATA_SIZE;

            memcpy(responseData, &demoPage[MAX_TCP_TX_DATA_SIZE], remainBytes);
            InsertDynamicValues(responseData, remainBytes);                   // exchange some strings...
            BuildTcpDataFrame(&tcpHeader, &txBuffer, httpServerConfig, responseData, remainBytes);
            SendTcpData(httpServerConfig, &txBuffer, remainBytes);
        }
    }
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
