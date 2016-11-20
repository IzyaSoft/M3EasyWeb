#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "driverConfig.h"

#define extern        // WTF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! (make this right)
#include "hal.h"
#include "networkManager.h"
#include "networkService.h"
#include "networkApplicationConfig.h"
#include "advancedTcpService.h"
#include "httpServer.h"

#include "main.h"     // for remove!

#define HTTP_PORT 80
#define TFTP_PORT 21

static void InitializeHttpConfig();
static void InitializeTftpConfig();
static void UpdateLeds();
void SysTickHandler();

struct NetworkApplicationConfig httpConfig;
struct NetworkApplicationConfig tftpConfig;
unsigned char ledsValue = 0xAA;
volatile uint32_t timeTick  = 0;

extern struct NetworkConfiguration networkConfiguration;
extern struct NetworkApplicationConfig* networkApplicationsConfig [] = {&httpConfig, &tftpConfig};
extern unsigned short numberOfConfigs = 2;

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
    unsigned char revertedMac[MAC_ADDRESS_LENGTH] = {networkConfiguration._macAddress[5], networkConfiguration._macAddress[4], networkConfiguration._macAddress[3],
                                                     networkConfiguration._macAddress[2], networkConfiguration._macAddress[1], networkConfiguration._macAddress[0]};
    ethernetConfiguration._macAddress = revertedMac;
    ethernetConfiguration._useAutoNegotiation = 1;

    InitializeNetwork(&ethernetConfiguration);
    InitializeHttpConfig();
    InitializeTftpConfig();
  
    while (1)
    {
        HandleNetworkEvents();
        if(!(httpConfig._isEnabled))
            OpenServer(&httpConfig);
        StartProcessing(GetEthernetBuffer());
    }
}

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
