#ifndef __TCPIP_H
#define __TCPIP_H

#include "hal.h"
#include "ethernetBuffer.h"
#include "ethernet.h"
#include "tcp.h"

#define MAX_LOCAL_BUFFER_SIZE_LIMIT                   127

extern unsigned char ethernetBuffer[MAX_ETH_FRAME_SIZE];

void InitializeNetwork(struct EthernetConfiguration* ethernetConfiguration);
void HandleNetworkEvents();
unsigned char CheckIsPacketBrodcast(struct EthernetBuffer* buffer);
void HandleBrodcastPacket(struct EthernetBuffer* buffer);
void HandleIndividualAddressPacket(struct EthernetBuffer* buffer);

#define RETRY_TIMEOUT        8                   // wait max. 8 x 262ms for an ACK (about 2 sec.)
#define FIN_TIMEOUT          2                   // max. time to wait for an ACK of a FIN
                                                 // before closing TCP state-machine (about 0.5 s)
#define MAX_RETRYS           4                   // nr. of resendings before reset conn.
                                                 // total nr. of transmissions = MAX_RETRYS + 1

#define MAX_TCP_TX_DATA_SIZE 512                 // max. outgoing TCP data size (even!)
#define MAX_TCP_RX_DATA_SIZE 256                 // max. incoming TCP data size (even!)
                                                 // (increasing the buffer-size dramatically
                                                 // increases the transfer-speed!)
                                        
#define MAX_ETH_TX_DATA_SIZE 60                  // 2nd buffer, used for ARP, ICMP, TCP (even!)
                                                 // enough to echo 32 byte via ICMP

#define IP_VER_IHL           0x4500              // IPv4, Header Length = 5x32 bit
#define IP_TOS_D             0x0010              // TOS low delay
#define IP_TOS_T             0x0008              // TOS high throughput
#define IP_TOS_R             0x0004              // TOS high reliability

#define IP_FLAG_DONTFRAG     0x4000              // don't fragment IP frame
#define IP_FLAG_MOREFRAG     0x2000              // more fragments available
#define IP_FRAGOFS_MASK      0x1FFF              // indicates where this fragment belongs

#define PROT_ICMP            1                   // Internet Control Message Protocol
#define PROT_TCP             6                   // Transmission Control Protocol
#define PROT_UDP             17                  // User Datagram Protocol

// TCP layer definitions
#define TCP_SRCPORT_OFS      IP_DATA_OFS + 0     // Source Port (16 bit)
#define TCP_DESTPORT_OFS     IP_DATA_OFS + 2     // Destination Port (16 bit)
#define TCP_SEQNR_OFS        IP_DATA_OFS + 4     // Sequence Number (32 bit)
#define TCP_ACKNR_OFS        IP_DATA_OFS + 8     // Acknowledge Number (32 bit)
#define TCP_DATA_CODE_OFS    IP_DATA_OFS + 12    // Data Offset and Control Bits (16 bit)
#define TCP_WINDOW_OFS       IP_DATA_OFS + 14    // Window Size (16 bit)
#define TCP_CHKSUM_OFS       IP_DATA_OFS + 16    // Checksum Field (16 bit)
#define TCP_URGENT_OFS       IP_DATA_OFS + 18    // Urgent Pointer (16 bit)
#define TCP_DATA_OFS         IP_DATA_OFS + 20    // Frame Data (if no options)
#define TCP_HEADER_SIZE      20                  // size w/o options

#define DATA_OFS_MASK        0xF000              // number of 32 bit words in the TCP Header

#define TCP_CODE_FIN         0x0001
#define TCP_CODE_SYN         0x0002
#define TCP_CODE_RST         0x0004
#define TCP_CODE_PSH         0x0008
#define TCP_CODE_ACK         0x0010
#define TCP_CODE_URG         0x0020

#define TCP_OPT_MSS          0x0204              // Type 2, Option Length 4 (Max. Segment Size)
#define TCP_OPT_MSS_SIZE     4

// define some TCP standard-ports, useful for testing...
#define TCP_PORT_ECHO        7                   // echo
#define TCP_PORT_DISCARD     9                   // discard
#define TCP_PORT_DAYTIME     13                  // daytime
#define TCP_PORT_QOTD        17                  // quote of the day
#define TCP_PORT_CHARGEN     19                  // character generator
#define TCP_PORT_HTTP        80                  // word wide web HTTP

#endif

