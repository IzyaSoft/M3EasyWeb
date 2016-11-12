#ifndef ICMP_H
#define ICMP_H

#include "ip.h"
#include "ethernetBuffer.h"
#include "ethernet.h"

#define ICMP_ECHO_REQUEST                             8
#define ICMP_ECHO_RESPONSE                            0

#define ICMP_HEADER_SIZE                              8

#define ICMP_TYPE_LENGTH                              1
#define ICMP_CODE_LENGTH                              1

#define ICMP_TYPE_INDEX                               IP_PACKET_DATA_INDEX
#define ICMP_CODE_INDEX                               ICMP_TYPE_INDEX + ICMP_TYPE_LENGTH
#define ICMP_CHEKSUM_INDEX                            ICMP_CODE_INDEX + ICMP_CODE_LENGTH

void BuildIcmpPacket(struct EthernetBuffer* buffer);

#endif
