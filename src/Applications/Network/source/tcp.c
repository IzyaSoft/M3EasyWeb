#include "tcp.h"
#include "networkUtils.h"

void ReadTcpHeader(struct EthernetBuffer* buffer, struct TcpHeader* tcpHeader)
{
    tchHeader->_sourcePort = ntohs(GetWord(buffer, TCP_SOURCE_PORT_INDEX));
    tchHeader->_destinationPort = ntohs(GetWord(buffer, TCP_DESTINATION_PORT_INDEX));
    tcpHeader->_sequenceNumber = ntohl(GetDoubleWord(buffer, TCP_SEQUENCE_NUMBER_INDEX));
    tcpHeader->_acknoledgemengtNumber = ntohl(GetDoubleWord(buffer, TCP_ACKNOWLEDGEMENT_NUMBER_INDEX));
    tcpHeader->_windowsSize = ntohs(GetWord(buffer, TCP_WINDOW_INDEX));
    tcpHeader->_urgency = ntohs(GetWord(buffer, TCP_URGENCY_INDEX));
    tcpHeader->_checkSum = ntohs(GetWord(buffer, TCP_CHECKSUM_INDEX));
    tcpHeader->_flags = ntohs(GetWord(buffer, TCP_DATA_CODE_INDEX));

    tcpHeader->_dataIndex = ((tcpHeader->_flags & 0x000F) * 4) - 1;
}
