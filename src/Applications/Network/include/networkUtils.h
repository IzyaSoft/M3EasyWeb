#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>
#include "ethernetBuffer.h"

void WriteWordAsBigEndian(unsigned char *address, unsigned short data);

unsigned short GetWord(struct EthernetBuffer* buffer, uint32_t index);
uint32_t GetDoubleWord(struct EthernetBuffer* buffer, uint32_t index);
void SetWord(unsigned short word, struct EthernetBuffer* buffer, uint32_t position);

uint32_t htonl(uint32_t value);
uint32_t ntohl(uint32_t value);
unsigned short htons(unsigned short value);
unsigned short ntohs(unsigned short value);

#endif
