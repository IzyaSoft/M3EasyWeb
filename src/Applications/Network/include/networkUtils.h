#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>
#include "ethernetBuffer.h"

void WriteWordAsBigEndian(unsigned char *address, unsigned short data);

unsigned short GetWord(struct EthernetBuffer* buffer, uint32_t index);
void SetWord(unsigned short word, struct EthernetBuffer* buffer, uint32_t position);

#endif
