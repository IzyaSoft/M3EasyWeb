#ifndef TCPSERVICECONTEXT_H
#define TCPSERVICECONTEXT_H

#include <stdint.h>

struct TcpHandshakeInfo
{
    //uint32_t _clockGeneratedSequenceNumber;
    uint32_t _sequenceNumber;                   // next packet sequence number
    uint32_t _acknowledgementNumber;             // next sequence to be received and ack to send
    uint32_t _unAcknowledgedSequenceNumber;     // not acknowledged sequence

};

#endif
