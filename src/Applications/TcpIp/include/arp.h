#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include "ethernetBuffer.h"
#include "ethernet.h"

#define ARP_HARDWARE_ADDRESS_TYPE_LENGTH      2
#define ARP_PROTOCOL_LENGTH                   2
#define ARP_HLEN_PLEN_LENGTH                  2
#define ARP_OPCODE_LENGTH                     2

#define ARP_HARDWARE_ADDRESS_TYPE_INDEX       ETHERNET_PAYLOAD_INDEX                                                // Hardware address type
#define ARP_PROTOCOL_INDEX                    ARP_HARDWARE_ADDRESS_TYPE_INDEX + ARP_HARDWARE_ADDRESS_TYPE_LENGTH    // Protocol
#define ARP_HLEN_PLEN_INDEX                   ARP_PROTOCOL_INDEX + ARP_PROTOCOL_LENGTH                              // Length of each hardware / prot. address
#define ARP_OPCODE_INDEX                      ARP_HLEN_PLEN_INDEX + ARP_HLEN_PLEN_LENGTH                            // Opcode
#define ARP_SENDER_MAC_INDEX                  ARP_OPCODE_INDEX + ARP_OPCODE_LENGTH                                  // Hardware address of sender of this packet
#define ARP_SENDER_IP_INDEX                   ARP_SENDER_MAC_INDEX + MAC_ADDRESS_LENGTH                             // IP address of sender
#define ARP_TARGET_MAC_INDEX                  ARP_SENDER_IP_INDEX + IPV4_LENGTH                                     // Hardware address of target of this packet
#define ARP_TARGET_IP_INDEX                   ARP_TARGET_MAC_INDEX + MAC_ADDRESS_LENGTH                             // IP address of target

#define HARDWARE_ETH10                        1                                                                     // hardware-type 10Mbps Ethernet
#define IPV4_HLEN_PLEN                        0x0604                                                                // MAC = 6 byte long, IP = 4 byte long
#define ARP_REQUEST_OPERATION                 1
#define ARP_REPLY_OPERATION                   2

void BuildArpReply(struct EthernetBuffer* buffer, unsigned char* macAddress, unsigned char* ipAddress);

#endif
