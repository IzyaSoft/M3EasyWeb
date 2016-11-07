#ifndef ETHERNET_H
#define ETHERNET_H

#define MAX_ETH_FRAME_SIZE                           1536
#define MAC_ADDRESS_LENGTH                              6
#define ETHER_TYPE_LENGTH                               2
#define IPV4_LENGTH                                     4

#define ETHERNET_DESTINATION_ADDRESS_INDEX              0
#define ETHERNET_SOURCE_ADDRESS_INDEX                   ETHERNET_DESTINATION_ADDRESS_INDEX + MAC_ADDRESS_LENGTH
#define ETHERNET_ETHERTYPE_INDEX                        ETHERNET_SOURCE_ADDRESS_INDEX + MAC_ADDRESS_LENGTH
#define ETHERNET_PAYLOAD_INDEX                          ETHERNET_ETHERTYPE_INDEX + ETHER_TYPE_LENGTH

#define SWAPBYTES(word)                                 ((unsigned short)((word) << 8) | ((word) >> 8)) // convert little <-> big endian
#endif
