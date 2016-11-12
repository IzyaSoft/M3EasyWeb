#ifndef IP_H
#define IP_H

#include "ethernet.h"
#include "ethernetBuffer.h"

#define ICMP_PROTOCOL                                                    1
#define TCP_PROTOCOL                                                     6
#define UDP_PROTOCOL                                                     17

#define IP_PACKET_SIZE_LENGTH                                            2
#define IP_PACKET_ID_LENGTH                                              2
#define IP_PACKET_FLAGS_LENGTH                                           2
#define IP_PACKET_TTL_LENGTH                                             1
#define IP_PACKET_PROTOCOL_LENGTH                                        1
#define IP_PACKET_HEADER_CHECKSUM_LENGTH                                 2

#define IP_PACKET_SIZE_INDEX                                             ETHERNET_PAYLOAD_INDEX + 2
#define IP_PACKET_ID_INDEX                                               IP_PACKET_SIZE_INDEX + IP_PACKET_SIZE_LENGTH
#define IP_PACKET_FLAGS_INDEX                                            IP_PACKET_ID_INDEX + IP_PACKET_ID_LENGTH
#define IP_PACKET_TTL_INDEX                                              IP_PACKET_FLAGS_INDEX + IP_PACKET_FLAGS_LENGTH
#define IP_PACKET_PROTOCOL_INDEX                                         IP_PACKET_TTL_INDEX + IP_PACKET_TTL_LENGTH
#define IP_PACKET_HEADER_CHECKSUM_INDEX                                  IP_PACKET_PROTOCOL_INDEX + IP_PACKET_PROTOCOL_LENGTH
#define IP_PACKET_HEADER_SOURCE_IP_INDEX                                 IP_PACKET_HEADER_CHECKSUM_INDEX + IP_PACKET_HEADER_CHECKSUM_LENGTH
#define IP_PACKET_HEADER_DESTINATION_IP_INDEX                            IP_PACKET_HEADER_SOURCE_IP_INDEX + IPV4_LENGTH
#define IP_PACKET_DATA_INDEX                                             IP_PACKET_HEADER_DESTINATION_IP_INDEX + IPV4_LENGTH

#define IP_HEADER_SIZE                                                   20

#define IP_TOS_D                                                         0x0010              // TOS low delay
#define IP_TOS_T                                                         0x0008              // TOS high throughput
#define IP_TOS_R                                                         0x0004              // TOS high reliability

unsigned short GetIpFrameSize(struct EthernetBuffer* buffer);
unsigned short GetIpChecksum(void *address, unsigned short count);
void InsertIpHeader(struct EthernetBuffer* buffer, unsigned short ipVersion, unsigned short packetSize, unsigned short packetId, unsigned short packetFlags, unsigned short ttl,
                    unsigned char* sourceIpAddress, unsigned char* destinationIpAddress);

#endif
