#include "tcp.h"
#include "networkUtils.h"
#include "networkConfiguration.h"

extern struct NetworkConfiguration networkConfiguration;

unsigned short GetTcpChecksum(void *address, unsigned short count, unsigned char* sourceIp, unsigned char* destinationIp)
{
    unsigned long checkSum = 0;
    unsigned short * addressShort = address;

    checkSum += ((unsigned short)sourceIp[1] << 8) + sourceIp[0];
    checkSum += ((unsigned short)sourceIp[3] << 8) + sourceIp[2];
    checkSum += ((unsigned short)destinationIp[1] << 8) + destinationIp[0];
    checkSum += ((unsigned short)destinationIp[3] << 8) + destinationIp[2];
    checkSum += htons(count);
    checkSum += htons(TCP_PROTOCOL);

    while (count > 1)
    {
        checkSum += *addressShort++;
        count -= 2;
    }

    if(count)                                     // add left-over byte, if any
        checkSum += *(unsigned char*)addressShort;

    while (checkSum >> 16)                        // fold 32-bit sum to 16 bits
        checkSum = (checkSum & 0xFFFF) + (checkSum >> 16);

    return ~checkSum;
}

void ReadTcpHeader(struct EthernetBuffer* buffer, struct TcpHeader* tcpHeader)
{
    tcpHeader->_sourcePort = ntohs(GetWord(buffer, TCP_SOURCE_PORT_INDEX));
    tcpHeader->_destinationPort = ntohs(GetWord(buffer, TCP_DESTINATION_PORT_INDEX));
    tcpHeader->_sequenceNumber = ntohl(GetDoubleWord(buffer, TCP_SEQUENCE_NUMBER_INDEX));
    tcpHeader->_acknoledgementNumber = ntohl(GetDoubleWord(buffer, TCP_ACKNOWLEDGEMENT_NUMBER_INDEX));
    tcpHeader->_windowsSize = ntohs(GetWord(buffer, TCP_WINDOW_INDEX));
    tcpHeader->_urgency = ntohs(GetWord(buffer, TCP_URGENCY_INDEX));
    tcpHeader->_checkSum = ntohs(GetWord(buffer, TCP_CHECKSUM_INDEX));
    tcpHeader->_flags = ntohs(GetWord(buffer, TCP_DATA_CODE_INDEX));

    tcpHeader->_dataIndex = ((tcpHeader->_flags & 0x000F) * 4) - 1;
}

void BuildTcpFrame(struct TcpHeader* tcpHeader, struct EthernetBuffer* buffer, unsigned short tcpCode, struct NetworkApplicationConfig* application)
{
    // Ethernet
    InsertEthernetHeader(buffer, networkConfiguration._macAddress, application->_client._macAddress, IP_ETHERTYPE);
    // IP
    unsigned short length = IP_HEADER_SIZE + TCP_HEADER_SIZE;
    if(tcpCode & TCP_CODE_SYN)
        length += TCP_OPT_MSS_SIZE;
    InsertIpHeader(buffer, IPV4_VERSION | IP_TOS_D, length, 0, 0, (TTL << 8) | TCP_PROTOCOL, networkConfiguration._ipAddress, application->_client._ipAddress);
    // TCP
    SetWord(htons(tcpHeader->_sourcePort),buffer, TCP_SOURCE_PORT_INDEX);
    SetWord(htons(tcpHeader->_destinationPort),buffer, TCP_DESTINATION_PORT_INDEX);
    SetDoubleWord(htonl(tcpHeader->_sequenceNumber), buffer, TCP_SEQUENCE_NUMBER_INDEX);
    SetDoubleWord(htonl(tcpHeader->_acknoledgementNumber), buffer, TCP_ACKNOWLEDGEMENT_NUMBER_INDEX);

    SetWord(htons(MAX_TCP_RX_DATA_SIZE), buffer, TCP_WINDOW_INDEX);
    SetWord(0, buffer, TCP_CHECKSUM_INDEX);
    SetWord(0, buffer, TCP_URGENCY_INDEX);

    if(tcpCode & TCP_CODE_SYN)
    {
        SetWord(htons(0x6000 | tcpCode), buffer, TCP_DATA_CODE_INDEX);
        SetWord(htons(TCP_OPT_MSS), buffer, TCP_DATA_INDEX);
        SetWord(htons(MAX_TCP_RX_DATA_SIZE), buffer, TCP_DATA_INDEX + 2);
        SetWord(GetTcpChecksum(&buffer->_buffer[TCP_SOURCE_PORT_INDEX], TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE, networkConfiguration._ipAddress, application->_client._ipAddress), buffer, TCP_CHECKSUM_INDEX);
    }
    else
    {
        SetWord(htons(0x5000 | tcpCode), buffer, TCP_DATA_CODE_INDEX);
        SetWord(GetTcpChecksum(&buffer->_buffer[TCP_SOURCE_PORT_INDEX], TCP_HEADER_SIZE, networkConfiguration._ipAddress, application->_client._ipAddress), buffer, TCP_CHECKSUM_INDEX);
    }

    buffer->_storedBytes = length += ETHERNET_HEADER_SIZE;
}
