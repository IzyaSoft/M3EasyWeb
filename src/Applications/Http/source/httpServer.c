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
}

void StartProcessing(struct EthernetBuffer* packedHttp)
{
    printf("HTTP Request received!\r\n");
    // 1. HTTP data parsing ...
    // 2. Find Handler ....
    // 3. Return result ....
    struct TcpHeader tcpHeader;
    ReadTcpHeader(packedHttp, &tcpHeader);
    uint32_t httpResponseSize = sizeof(demoPage) - 1;
    unsigned char httpHeaderSent = 0;
    unsigned char *dataPtr;

    dataPtr = demoPage;
    struct EthernetBuffer txBuffer;
    unsigned char txBufferStorage[1536];
    txBuffer._buffer = txBufferStorage;
    txBuffer._bufferCapacity = 1536;
    unsigned short length = 0;
    //if(httpResponseSize > MAX_TCP_TX_DATA_SIZE)
    //{
    //if(!httpHeaderSent)
    //{
    memcpy(txBufferStorage, demoResponseHeader, sizeof(demoResponseHeader) - 1);
    length += sizeof(demoResponseHeader) - 1;
    memcpy(txBufferStorage + sizeof(demoResponseHeader) - 1, dataPtr, MAX_TCP_TX_DATA_SIZE - sizeof(demoResponseHeader) + 1);
    length += MAX_TCP_TX_DATA_SIZE - sizeof(demoResponseHeader) + 1;
    //httpResponseSize -= MAX_TCP_TX_DATA_SIZE - sizeof(demoResponseHeader) + 1;
    dataPtr += MAX_TCP_TX_DATA_SIZE - sizeof(demoResponseHeader) + 1;
    //}
    //else
    //{
    //memcpy(txBufferStorage, dataPtr, MAX_TCP_TX_DATA_SIZE);
    //httpResponseSize -= MAX_TCP_TX_DATA_SIZE;
    //dataPtr += MAX_TCP_TX_DATA_SIZE;
    //}

    InsertDynamicValues(txBufferStorage, length);                   // exchange some strings...
    BuildTcpDataFrame(&tcpHeader, &txBuffer, httpServerConfig, txBufferStorage,  length);
    TransmitData(&txBuffer);                   // xfer buffer
    httpServerConfig->_context._unAcknowledgedSequenceNumber += length;
    //if(httpResponseSize)                  // transmit leftover bytes
    //{
    memcpy(txBufferStorage, dataPtr, sizeof(demoPage) - length);
    BuildTcpDataFrame(&tcpHeader, &txBuffer, httpServerConfig, txBufferStorage, sizeof(demoPage) - length);
    InsertDynamicValues(txBufferStorage, sizeof(demoPage) - length);                   // exchange some strings...
    TransmitData(&txBuffer);                   // send last segment
    //httpServerConfig->_context._unAcknowledgedSequenceNumber += sizeof(demoPage) - length;
            //TCPClose();                              // and close connection
    //httpResponseSize = 0;                     // all data sent
    //}
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
                        //GetAD7Val();                  // get AD value
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
