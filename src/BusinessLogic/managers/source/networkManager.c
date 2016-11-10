#include "networkManager.h"



extern unsigned char macAddress[] =  {0x1E, 0x30, 0x6C, 0xA2, 0x45, 0x5E};
extern unsigned char revertedMacAddress[] =  {0x5E, 0x45, 0xA2, 0x6C, 0x30, 0x1E};
extern unsigned char defaultIpAddress[] = {192, 168, 200, 5};
extern unsigned char defaultNetmask[] = {255, 255, 255, 0};
extern unsigned char defaultGateway[] = {192, 168, 200, 1};

void GetNetworkConfiguration(struct NetworkConfiguration* networkConfiguration)
{
    //todo umv: mount fs, read network settings from network.cfg

    networkConfiguration->_isStaticIp = 1;
    networkConfiguration->_macAddress = macAddress;
    networkConfiguration->_revertedMacAddress = revertedMacAddress;
    networkConfiguration->_ipAddress = defaultIpAddress;
    networkConfiguration->_netmask = defaultNetmask;
    networkConfiguration->_gateway = defaultGateway;
}
