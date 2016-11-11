#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "ip.h"
#include "tcpHeader.h"

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

void ReadTcpHeader(struct EthernetBuffer* buffer, struct TcpHeader* tcpHeader);

#endif
