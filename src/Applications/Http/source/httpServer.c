#include "hal.h"
#include "tcp.h"
#include "tcpHeader.h"
#include "advancedTcpService.h"
#include "httpServer.h"
#include "networkConfiguration.h"

#include "demopage.h"
#include "demoheader.h"

unsigned int pagecounter = 100;
unsigned int adcValue = 0;

struct NetworkApplicationConfig* httpServerConfig;
extern struct NetworkConfiguration networkConfiguration;

void OpenServer(struct NetworkApplicationConfig* config)
{
    httpServerConfig = config;
    httpServerConfig->_isEnabled = 1;
    for(unsigned char clientCounter = 0; clientCounter < MAX_CLIENTS_NUMBER; clientCounter++)
        httpServerConfig->_client[clientCounter]._tcpState = LISTENING;
    //remove lines below
/*    if(config->_tcpState == CLOSED)
    {
        config->_tcpFlags &= ~TCP_ACTIVE_OPEN;
        config->_tcpState = LISTENING;
        config->_socketStatus = SOCK_ACTIVE;
    }*/
    //httpServerConfig->_client._handshakeInfo._sequenceNumber = 0;
    //httpServerConfig->_client._handshakeInfo._acknowledgementNumber = 0;
    //httpServerConfig->_client._handshakeInfo._unAcknowledgedSequenceNumber = 0;
}

void StartProcessing(struct EthernetBuffer* packedHttp)
{
    //if(httpServerConfig->_socketStatus & SOCK_DATA_AVAILABLE)
    //{
        //printf("HTTP Request received!\r\n");
        //httpServerConfig->_socketStatus &= ~SOCK_DATA_AVAILABLE;
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
            struct TcpHeader txHeader;
            if(!firstPartSent)
            {
            memcpy(responseData, demoResponseHeader, demoHeaderLength);
            memcpy(&responseData[demoHeaderLength], demoPage, MAX_TCP_TX_DATA_SIZE - demoHeaderLength);

            httpServerConfig->_client[0]._handshakeInfo._acknowledgementNumber -= httpServerConfig->_client[0]._dataLength;
            InsertDynamicValues(responseData,  MAX_TCP_TX_DATA_SIZE);

            txHeader._sourcePort = httpServerConfig->_applicationPort;
            txHeader._destinationPort = httpServerConfig->_client[0]._tcpPort;
            txHeader._sequenceNumber = httpServerConfig->_client[0]._handshakeInfo._sequenceNumber;// + MAX_TCP_TX_DATA_SIZE;
            txHeader._acknowledgementNumber = httpServerConfig->_client[0]._handshakeInfo._acknowledgementNumber;//._unAcknowledgedSequenceNumber;
            //httpServerConfig->_client[0]._handshakeInfo._acknowledgementNumber += httpServerConfig->_client[0]._dataLength;
            txHeader._windowsSize = MAX_TCP_TX_DATA_SIZE;
            txHeader._urgency = 0;
            txHeader._flags = TCP_CODE_ACK;
            memcpy(txHeader._destinationIpAddress, httpServerConfig->_client[0]._ipAddress, IPV4_LENGTH);
            memcpy(txHeader._destinationMacAddress, httpServerConfig->_client[0]._macAddress, MAC_ADDRESS_LENGTH);
            memcpy(txHeader._sourceIpAddress, networkConfiguration._ipAddress, IPV4_LENGTH);
            memcpy(txHeader._sourceMacAddress, networkConfiguration._macAddress, MAC_ADDRESS_LENGTH);
            httpServerConfig->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber = httpServerConfig->_client[0]._handshakeInfo._sequenceNumber;
            //BuildTcpDataFrame(&txBuffer, httpServerConfig->_applicationPort, &httpServerConfig->_client[0], responseData, MAX_TCP_TX_DATA_SIZE);
            BuildTcpPacket(&txBuffer, &txHeader, responseData, MAX_TCP_TX_DATA_SIZE);
            printf("HTTP send data %d\r\n", (int)MAX_TCP_TX_DATA_SIZE);
            SendTcpData(&txBuffer);
            firstPartSent = 1;
            //httpServerConfig->_client[0]._handshakeInfo._sequenceNumber += MAX_TCP_TX_DATA_SIZE;
            return;
            }
            else
            {
            //if(httpServerConfig->_client[0]._handshakeInfo._acknowledgementNumber < httpServerConfig->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber)
              //  return;
            unsigned short remainBytes = demoPageLength - MAX_TCP_TX_DATA_SIZE;

            memcpy(responseData, &demoPage[MAX_TCP_TX_DATA_SIZE], remainBytes);
            InsertDynamicValues(responseData, remainBytes);
            printf("HTTP send data %d\r\n", (int)remainBytes);
            //httpServerConfig->_client[0]._handshakeInfo._acknowledgementNumber +=remainBytes;
            txHeader._sourcePort = httpServerConfig->_applicationPort;
            txHeader._destinationPort = httpServerConfig->_client[0]._tcpPort;
            txHeader._sequenceNumber = httpServerConfig->_client[0]._handshakeInfo._sequenceNumber;
            txHeader._acknowledgementNumber = httpServerConfig->_client[0]._handshakeInfo._acknowledgementNumber;
            httpServerConfig->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber = httpServerConfig->_client[0]._handshakeInfo._sequenceNumber;
            txHeader._windowsSize = MAX_TCP_TX_DATA_SIZE;
            txHeader._urgency = 0;
            txHeader._flags = TCP_CODE_ACK;;
            memcpy(txHeader._destinationIpAddress, httpServerConfig->_client[0]._ipAddress, IPV4_LENGTH);
            memcpy(txHeader._destinationMacAddress, httpServerConfig->_client[0]._macAddress, MAC_ADDRESS_LENGTH);
            memcpy(txHeader._sourceIpAddress, networkConfiguration._ipAddress, IPV4_LENGTH);
            memcpy(txHeader._sourceMacAddress, networkConfiguration._macAddress, MAC_ADDRESS_LENGTH);
            //= httpServerConfig->_client[0]._handshakeInfo._sequenceNumber;
            //BuildTcpDataFrame(&txBuffer, httpServerConfig->_applicationPort, &httpServerConfig->_client[0], responseData, remainBytes);
            BuildTcpPacket(&txBuffer, &txHeader, responseData, remainBytes);
            SendTcpData(&txBuffer);
            		//httpServerConfig, &txBuffer, remainBytes, 1);
            firstPartSent = 0;
            httpServerConfig->_client[0]._hasData = 0;
            httpServerConfig->_client[0]._socketStatus = SOCKET_CLOSING;
            return;
            //httpServerConfig->_client[0]._handshakeInfo._sequenceNumber += remainBytes;
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
