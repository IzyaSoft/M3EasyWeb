#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <stdint.h>
#include "networkConfiguration.h"

extern unsigned char macAddress[];
extern unsigned char defaultIpAddress[];
extern unsigned char defaultNetmask[];
extern unsigned char defaultGateway[];

void GetNetworkConfiguration(struct NetworkConfiguration* networkConfiguration);

#endif
