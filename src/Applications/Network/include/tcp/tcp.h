#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "ip.h"
#include "tcpHeader.h"
#include "networkApplicationConfig.h"

#define TCP_PORT_LENGTH                                      2
#define TCP_SEQUENCE_NUMBER_LENGTH                           4
#define TCP_ACKNOWLEDGEMENT_NUMBER_LENGTH                    4
#define TCP_DATA_CODE_LENGTH                                 2
#define TCP_WINDOW_LENGTH                                    2
#define TCP_CHECKSUM_LENGTH                                  2
#define TCP_URGENCY_LENGTH                                   2

#define TCP_SOURCE_PORT_INDEX                                IP_PACKET_DATA_INDEX + 0                                                   // Source Port (16 bit)
#define TCP_DESTINATION_PORT_INDEX                           TCP_SOURCE_PORT_INDEX + TCP_PORT_LENGTH                                    // Destination Port (16 bit)
#define TCP_SEQUENCE_NUMBER_INDEX                            TCP_DESTINATION_PORT_INDEX + TCP_PORT_LENGTH                               // Sequence Number (32 bit)
#define TCP_ACKNOWLEDGEMENT_NUMBER_INDEX                     TCP_SEQUENCE_NUMBER_INDEX + TCP_SEQUENCE_NUMBER_LENGTH                     // Acknowledge Number (32 bit)
#define TCP_DATA_CODE_INDEX                                  TCP_ACKNOWLEDGEMENT_NUMBER_INDEX + TCP_ACKNOWLEDGEMENT_NUMBER_LENGTH       // Data Offset and Control Bits (16 bit)
#define TCP_WINDOW_INDEX                                     TCP_DATA_CODE_INDEX + TCP_DATA_CODE_LENGTH                                 // Window Size (16 bit)
#define TCP_CHECKSUM_INDEX                                   TCP_WINDOW_INDEX + TCP_WINDOW_LENGTH                                       // Checksum Field (16 bit)
#define TCP_URGENCY_INDEX                                    TCP_CHECKSUM_INDEX + TCP_CHECKSUM_LENGTH                                   // Urgent Pointer (16 bit)
#define TCP_DATA_INDEX                                       TCP_URGENCY_INDEX + TCP_URGENCY_LENGTH

enum TcpState                                                                                                                           // states of the TCP-state machine according to RFC793
{
    CLOSED = 1,
    LISTENING,
    SYN_SENT,
    SYN_RECD,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT
};

#define TCP_CODE_FIN                                         0x0001
#define TCP_CODE_SYN                                         0x0002
#define TCP_CODE_RST                                         0x0004
#define TCP_CODE_PSH                                         0x0008
#define TCP_CODE_ACK                                         0x0010
#define TCP_CODE_URG                                         0x0020

#define TCP_HEADER_SIZE                                      20
#define TCP_OPT_MSS_SIZE                                     4
#define TCP_OPT_MSS                                          0x0204                 // Type 2, Option Length 4 (Max. Segment Size)

#define MAX_TCP_TX_DATA_SIZE                                 1024                   // max. outgoing TCP data size (even!)
#define MAX_TCP_RX_DATA_SIZE                                 1024                   // max. incoming TCP data size (even!)

//unsigned short GetTcpChecksum(void *address, unsigned short count, unsigned char* sourceIp, unsigned char* destinationIp);
struct TcpHeader ReadTcpHeader(struct EthernetBuffer* buffer);
void BuildTcpFrame(struct EthernetBuffer* buffer, unsigned short tcpCode, unsigned short applicationPort, struct NetworkApplicationClient* client);
void BuildTcpDataFrame(struct EthernetBuffer* buffer, unsigned short applicationPort, struct NetworkApplicationClient* client, unsigned char* tcpData, unsigned short tcpDataLength);

#endif
