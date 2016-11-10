#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>

//void MakeNetworkBytesOrder(unsigned char* buffer, uint32_t length);
//void MakeMcuBytesOrder(unsigned char* buffer, uint32_t length);
void WriteWordAsBigEndian(unsigned char *address, unsigned short data);

#endif
