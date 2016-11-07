#include "networkManager.h"

extern unsigned char* macAddress = {0x00, 0x33, 0x11, 0x66, 0x22, 0xEE};
extern unsigned char* defaultIpAddress = {192, 168, 200, 5};
extern unsigned char* defaultNetmask = {255, 255, 255, 0};
extern unsigned char* defaultGateway = {192, 168, 200, 1};

void GetNetworkConfiguration(struct NetworkConfiguration* networkConfiguration)
{
    //todo umv: mount fs, read settings

    networkConfiguration->_isStaticIp = 1;
    networkConfiguration->_macAddress = macAddress;
    networkConfiguration->_ipAddress = defaultIpAddress;
    networkConfiguration->_netmask = defaultNetmask;
    networkConfiguration->_gateway = defaultGateway;
}
