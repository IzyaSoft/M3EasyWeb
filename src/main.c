﻿#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "driverConfig.h"

#define extern        // WTF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! (make this nice!!!!!!)
#include "hal.h"
#include "networkManager.h"
#include "networkService.h"
#include "networkApplicationConfig.h"
#include "tcpService.h"
#include "httpServer.h"

#include "main.h"     // for remove!
#include "webpage.h"

#define HTTP_PORT 80
#define TFTP_PORT 21

static void InitializeHttpConfig();
static void InitializeTftpConfig();
static void UpdateLeds();
void SysTickHandler();

struct NetworkApplicationConfig httpConfig;
struct NetworkApplicationConfig tftpConfig;

extern struct NetworkConfiguration networkConfiguration;
extern struct NetworkApplicationConfig* networkApplicationsConfig [] = {&httpConfig, &tftpConfig};
extern unsigned short numberOfConfigs = 2;

unsigned int pagecounter = 100;
unsigned int adcValue = 0;
unsigned char ledsValue = 0xAA;
volatile uint32_t timeTick  = 0;

int main()
{
    // Clocks initialization
    ConfigureSystemClock();
    SetSysTickHandler(&SysTickHandler, 100);  // every 10 ms
    // Modules configuration
    ConfigureLedPort(LED_PORT_NUMBER, LED_PORT_MASK, ledsValue);
    ConfigureAdc(ADC_CHANNEL_5, ADC_CLOCK_DIVIDER);
    // Networks
    struct EthernetConfiguration ethernetConfiguration;
    GetNetworkConfiguration(&networkConfiguration);
    ethernetConfiguration._macAddress = networkConfiguration._macAddress;
    ethernetConfiguration._useAutoNegotiation = 1;

    InitializeNetwork(&ethernetConfiguration);
    InitializeHttpConfig();
    InitializeTftpConfig();

/*
  *(unsigned char *)RemoteIP = 24;               // uncomment those lines to get the
  *((unsigned char *)RemoteIP + 1) = 8;          // quote of the day from a real
  *((unsigned char *)RemoteIP + 2) = 69;         // internet server! (gateway must be
  *((unsigned char *)RemoteIP + 3) = 7;          // set to your LAN-router)

  TCPLocalPort = 2025;
  TCPRemotePort = TCP_PORT_QOTD;

  TCPActiveOpen();

  while (SocketStatus & SOCK_ACTIVE)             // read the quote from memory
  {                                              // by using the hardware-debugger
    DoNetworkStuff();
  }
*/




    //HTTPStatus = 0;                                // clear HTTP-server's flag register
    //TCPLocalPort = TCP_PORT_HTTP;                  // set port we want to listen to
  
    while (1)                                      // repeat forever
    {
        HandleNetworkEvents();
        if(!(httpConfig._socketStatus & SOCK_ACTIVE))
            OpenServer(&httpConfig, 0);
        if( HasApplicationData())
            StartProcessing(GetEthernetBuffer());
    	//DoNetworkStuff();
        /*if (!(SocketStatus & SOCK_ACTIVE))
            TCPPassiveOpen();   // listen for incoming TCP-connection
        DoNetworkStuff();                                  // handle network and easyWEB-stack
                                                           // events
        HTTPServer();*/
    }
}

// This function implements a very simple dynamic HTTP-server.
// It waits until connected, then sends a HTTP-header and the
// HTML-code stored in memory. Before sending, it replaces
// some special strings with dynamic values.
// NOTE: For strings crossing page boundaries, replacing will
// not work. In this case, simply add some extra lines
// (e.g. CR and LFs) to the HTML-code.

/*void HTTPServer(void)
{
    if (SocketStatus & SOCK_CONNECTED)             // check if somebody has connected to our TCP
    {
        if (SocketStatus & SOCK_DATA_AVAILABLE)      // check if remote TCP sent data
           TCPReleaseRxBuffer();                      // and throw it away

        if (SocketStatus & SOCK_TX_BUF_RELEASED)     // check if buffer is free for TX
        {
            if (!(HTTPStatus & HTTP_SEND_PAGE))        // init byte-counter and pointer to webside
            {                                          // if called the 1st time
                HTTPBytesToSend = sizeof(WebSide) - 1;   // get HTML length, ignore trailing zero
                PWebSide = (unsigned char *)WebSide;     // pointer to HTML-code
            }

            if (HTTPBytesToSend > MAX_TCP_TX_DATA_SIZE)     // transmit a segment of MAX_SIZE
            {
                if (!(HTTPStatus & HTTP_SEND_PAGE))           // 1st time, include HTTP-header
                {
                    memcpy(TCP_TX_BUF, GetResponse, sizeof(GetResponse) - 1);
                    memcpy(TCP_TX_BUF + sizeof(GetResponse) - 1, PWebSide, MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1);
                    HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
                    PWebSide += MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
                }
                else
                {
                    memcpy(TCP_TX_BUF, PWebSide, MAX_TCP_TX_DATA_SIZE);
                    HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE;
                    PWebSide += MAX_TCP_TX_DATA_SIZE;
                }

                TCPTxDataCount = MAX_TCP_TX_DATA_SIZE;   // bytes to xfer
                InsertDynamicValues();                   // exchange some strings...
                TCPTransmitTxBuffer();                   // xfer buffer
            }
            else if (HTTPBytesToSend)                  // transmit leftover bytes
            {
                memcpy(TCP_TX_BUF, PWebSide, HTTPBytesToSend);
                TCPTxDataCount = HTTPBytesToSend;        // bytes to xfer
                InsertDynamicValues();                   // exchange some strings...
                TCPTransmitTxBuffer();                   // send last segment
                TCPClose();                              // and close connection
                HTTPBytesToSend = 0;                     // all data sent
            }

            HTTPStatus |= HTTP_SEND_PAGE;              // ok, 1st loop executed
        }
    }
    else HTTPStatus &= ~HTTP_SEND_PAGE;               // reset help-flag if not connected
}

// searches the TX-buffer for special strings and replaces them
// with dynamic values (AD-converter results)

void InsertDynamicValues(void)
{
    unsigned char *Key;
    char NewKey[5];
    unsigned int i;
  
    if (TCPTxDataCount < 4) return;                     // there can't be any special string
  
    Key = TCP_TX_BUF;
  
    for (i = 0; i < (TCPTxDataCount - 3); i++)
    {
        if (*Key == 'A')
            if (*(Key + 1) == 'D')
                if (*(Key + 3) == '%')
                switch (*(Key + 2))
                {
                    case '8' :                                 // "AD8%"?
                    {
                        adcValue = GetAdcValue(ADC_CHANNEL_5);
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
}*/

static void InitializeHttpConfig()
{
    httpConfig._applicationPort = HTTP_PORT;
    httpConfig._isTcpApplication = 1;
    httpConfig._tcpState = CLOSED;
}

static void InitializeTftpConfig()
{
    tftpConfig._applicationPort = TFTP_PORT;
    tftpConfig._isTcpApplication = 1;
    tftpConfig._tcpState = CLOSED;
}

static void UpdateLeds()
{
    ledsValue = 0xFF - ledsValue;
    SetLedsValue(LED_PORT_NUMBER, ledsValue);
}

void SysTickHandler ()
{
    timeTick++;
    if (timeTick >= 20)
    {
    	UpdateLeds();
        timeTick = 0;
        if((timeTick % 10) == 0)
            HandleNetworkServiceClockTick(timeTick);
        HandleTcpServiceClockTick();
    }
}
