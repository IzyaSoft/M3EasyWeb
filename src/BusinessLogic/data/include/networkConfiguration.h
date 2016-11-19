#ifndef NETWORKCONFIGURATION_H
#define NETWORKCONFIGURATION_H

struct NetworkConfiguration
{
	unsigned char _isStaticIp;

    unsigned char* _macAddress;
    unsigned char* _ipAddress;
    unsigned char* _netmask;
    unsigned char* _gateway;

    unsigned char* _dhcpServer;

    unsigned char* _nameServer;
};

#endif
