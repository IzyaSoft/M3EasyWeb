#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>
#include "ethernetBuffer.h"

unsigned short GetWord(struct EthernetBuffer* buffer, uint32_t index);
uint32_t GetDoubleWord(struct EthernetBuffer* buffer, uint32_t index);
void SetWord(unsigned short word, struct EthernetBuffer* buffer, uint32_t position);
void SetDoubleWord(uint32_t dword, struct EthernetBuffer* buffer, uint32_t position);

unsigned char CheckIpAddressEquals(unsigned char* ipAddress1, unsigned char* ipAddress2);
unsigned char CheckMacAddressEquals(unsigned char* macAddress1, unsigned char* macAddress2);

uint32_t htonl(uint32_t value);
uint32_t ntohl(uint32_t value);
unsigned short htons(unsigned short value);
unsigned short ntohs(unsigned short value);

#endif
