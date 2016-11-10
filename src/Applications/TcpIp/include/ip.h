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

//unsigned short GetIpHeaderSize(struct EthernetBuffer* buffer);
unsigned short GetIpFrameSize(struct EthernetBuffer* buffer);
unsigned short GetChecksumForNonTcpPackets(void *address, unsigned short count);

#endif
