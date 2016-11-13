#ifndef TCPSERVICE_H
#define TCPSERVICE_H
#include "networkService.h"

#define TCP_ACTIVE_OPEN                                 0x01
#define TCP_TIMER_RUNNING                               0x04
#define TIMER_TYPE_RETRY                                0x08
#define TCP_CLOSE_REQUESTED                             0x10

#define MAX_RETRIES                                     4
#define RETRY_TIMEOUT                                   8         // wait max. 8 x 262ms for an ACK (about 2 sec.)
#define FIN_TIMEOUT                                     2         // max. time to wait for an ACK of a FIN

#define SOCK_ACTIVE                                     0x01      // state machine NOT closed
#define SOCK_CONNECTED                                  0x02      // user may send & receive data
#define SOCK_DATA_AVAILABLE                             0x04      // new data available
#define SOCK_TX_BUF_RELEASED                            0x08      // user may fill buffer

#define SOCK_ERROR_MASK                                 0xF0      // bit-mask to check for errors
#define SOCK_ERR_OK                                     0x00      // no error
#define SOCK_ERR_ARP_TIMEOUT                            0x10      // timeout waiting for an ARP-REPLY
#define SOCK_ERR_TCP_TIMEOUT                            0x20      // timeout waiting for an ACK
#define SOCK_ERR_CONN_RESET                             0x30      // connection was reset by the other TCP
#define SOCK_ERR_REMOTE                                 0x40      // remote TCP caused fatal error
#define SOCK_ERR_ETHERNET                               0x50      // network interface error (timeout)

void HandleTcpPacket(struct EthernetBuffer* buffer);
void HandleTcpServiceClockTick();
#endif
