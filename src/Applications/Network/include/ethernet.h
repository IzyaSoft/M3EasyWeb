#ifndef ETHERNET_H
#define ETHERNET_H

#define MAX_ETH_FRAME_SIZE                              1536
#define MAC_ADDRESS_LENGTH                              6
#define ETHER_TYPE_LENGTH                               2
#define IPV4_LENGTH                                     4

#define ETHERNET_DESTINATION_ADDRESS_INDEX              0
#define ETHERNET_SOURCE_ADDRESS_INDEX                   ETHERNET_DESTINATION_ADDRESS_INDEX + MAC_ADDRESS_LENGTH
#define ETHERNET_ETHERTYPE_INDEX                        ETHERNET_SOURCE_ADDRESS_INDEX + MAC_ADDRESS_LENGTH
#define ETHERNET_PAYLOAD_INDEX                          ETHERNET_ETHERTYPE_INDEX + ETHER_TYPE_LENGTH

#define ARP_ETHERTYPE                                   0x0806
#define IP_ETHERTYPE                                    0x0800

#define ETHERNET_HEADER_SIZE                            14
#define ARP_PACKET_SIZE                                 28

#define IPV4_VERSION                                    0x4500     // 5 - FOR IP HEADER SIZE

#define SWAPBYTES(word)                                 ((unsigned short)((word) << 8) | ((word) >> 8)) // convert little <-> big endian
#endif
