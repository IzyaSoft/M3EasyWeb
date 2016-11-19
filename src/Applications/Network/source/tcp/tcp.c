#include "tcp.h"
#include "networkUtils.h"
#include "networkConfiguration.h"

static void PrintTcpHeader(struct TcpHeader* tcpHeader);

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

//todo: umv: maybe we should return tcpHeader as result (maybe we should dynamically create TcpHeader objects)
void ReadTcpHeader(struct EthernetBuffer* buffer, struct TcpHeader* tcpHeader)
{
    tcpHeader->_sourcePort = GetWord(buffer, TCP_SOURCE_PORT_INDEX);
    tcpHeader->_destinationPort = GetWord(buffer, TCP_DESTINATION_PORT_INDEX);
    tcpHeader->_sequenceNumber = GetDoubleWord(buffer, TCP_SEQUENCE_NUMBER_INDEX);
    tcpHeader->_acknowledgementNumber =GetDoubleWord(buffer, TCP_ACKNOWLEDGEMENT_NUMBER_INDEX);
    tcpHeader->_windowsSize = GetWord(buffer, TCP_WINDOW_INDEX);
    tcpHeader->_urgency = GetWord(buffer, TCP_URGENCY_INDEX);
    tcpHeader->_checkSum = GetWord(buffer, TCP_CHECKSUM_INDEX);
    unsigned short flagsCode = GetWord(buffer, TCP_DATA_CODE_INDEX);
    tcpHeader->_flags = (unsigned char)flagsCode;//);
    tcpHeader->_headerSize = ((flagsCode & 0xF000) >> 12) * 4;
    tcpHeader->_dataIndex = tcpHeader->_headerSize - 1;
    // PrintTcpHeader(tcpHeader);
}

void BuildTcpFrame(struct TcpHeader* tcpHeader, struct EthernetBuffer* buffer, unsigned short tcpCode, struct NetworkApplicationConfig* application)
{
    // Ethernet
    InsertEthernetHeader(buffer, networkConfiguration._macAddress, application->_client._macAddress, IP_ETHERTYPE);
    // IP
    unsigned short length = IP_HEADER_SIZE + TCP_HEADER_SIZE;
    if(tcpCode & TCP_CODE_SYN)
    {
        length += TCP_OPT_MSS_SIZE;
    }
    InsertIpHeader(buffer, IPV4_VERSION | IP_TOS_D, length, 0, 0, (TTL << 8) | TCP_PROTOCOL, networkConfiguration._ipAddress, application->_client._ipAddress);
    // TCP
    SetWord(application->_applicationPort, buffer, TCP_SOURCE_PORT_INDEX);
    SetWord(tcpHeader->_sourcePort,buffer, TCP_DESTINATION_PORT_INDEX);
    SetDoubleWord(application->_context._sequenceNumber, buffer, TCP_SEQUENCE_NUMBER_INDEX);
    SetDoubleWord(application->_context._acknowledgementNumber, buffer, TCP_ACKNOWLEDGEMENT_NUMBER_INDEX);

    SetWord(MAX_TCP_RX_DATA_SIZE, buffer, TCP_WINDOW_INDEX);
    SetWord(0, buffer, TCP_CHECKSUM_INDEX);
    SetWord(0, buffer, TCP_URGENCY_INDEX);

    if(tcpCode & TCP_CODE_SYN)
    {
        SetWord(0x6000 | tcpCode, buffer, TCP_DATA_CODE_INDEX);
        SetWord(TCP_OPT_MSS, buffer, TCP_DATA_INDEX);
        SetWord(MAX_TCP_RX_DATA_SIZE, buffer, TCP_DATA_INDEX + 2);
        SetWord(htons(GetTcpChecksum(&buffer->_buffer[TCP_SOURCE_PORT_INDEX], TCP_HEADER_SIZE + TCP_OPT_MSS_SIZE, networkConfiguration._ipAddress, application->_client._ipAddress)), buffer, TCP_CHECKSUM_INDEX);
    }
    else
    {
        SetWord(0x5000 | tcpCode, buffer, TCP_DATA_CODE_INDEX);
        SetWord(htons(GetTcpChecksum(&buffer->_buffer[TCP_SOURCE_PORT_INDEX], TCP_HEADER_SIZE, networkConfiguration._ipAddress, application->_client._ipAddress)), buffer, TCP_CHECKSUM_INDEX);
    }

    buffer->_storedBytes = length += ETHERNET_HEADER_SIZE;
}

void BuildTcpDataFrame(struct TcpHeader* tcpHeader, struct EthernetBuffer* buffer, struct NetworkApplicationConfig* application, unsigned char* dataBuffer, unsigned short dataBufferLength)
{
    // Ethernet
    InsertEthernetHeader(buffer, networkConfiguration._macAddress, application->_client._macAddress, IP_ETHERTYPE);
    // IP
    unsigned short length = IP_HEADER_SIZE + TCP_HEADER_SIZE + dataBufferLength;
    InsertIpHeader(buffer, IPV4_VERSION | IP_TOS_D, length, 0, 0, (TTL << 8) | TCP_PROTOCOL, networkConfiguration._ipAddress, application->_client._ipAddress);
    // TCP
    SetWord(application->_applicationPort, buffer, TCP_SOURCE_PORT_INDEX);
    SetWord(tcpHeader->_sourcePort,buffer, TCP_SOURCE_PORT_INDEX);
    SetDoubleWord(application->_context._sequenceNumber, buffer, TCP_SEQUENCE_NUMBER_INDEX);
    SetDoubleWord(application->_context._acknowledgementNumber, buffer, TCP_ACKNOWLEDGEMENT_NUMBER_INDEX);

    SetWord(MAX_TCP_RX_DATA_SIZE, buffer, TCP_WINDOW_INDEX);
    SetWord(0, buffer, TCP_CHECKSUM_INDEX);
    SetWord(0, buffer, TCP_URGENCY_INDEX);
    SetWord(0x5000 | TCP_CODE_ACK, buffer, TCP_DATA_CODE_INDEX);
    memcpy(&buffer->_buffer[TCP_DATA_INDEX], dataBuffer, dataBufferLength);
    SetWord(htons(GetTcpChecksum(&buffer->_buffer[TCP_SOURCE_PORT_INDEX], TCP_HEADER_SIZE + dataBufferLength, networkConfiguration._ipAddress, application->_client._ipAddress)), buffer, TCP_CHECKSUM_INDEX);
    buffer->_storedBytes = length += ETHERNET_HEADER_SIZE;
}

static void PrintTcpHeader(struct TcpHeader* tcpHeader)
{
    printf("TCP received and parsed\r\n");
    printf("TCP Header\r\n");
    printf("Source port: %d\r\n", tcpHeader->_sourcePort);
    printf("Destination port: %d\r\n", tcpHeader->_destinationPort);

    printf("Header size: %d\r\n", tcpHeader->_headerSize);
    printf("Flags: %d\r\n", tcpHeader->_flags);

    printf("Sequence number: %d\r\n", tcpHeader->_sequenceNumber);
    printf("Acknowledgement number: %d\r\n", tcpHeader->_acknowledgementNumber);

    printf("Urgency: %d\r\n", tcpHeader->_urgency);
    printf("Window size: %d\r\n", tcpHeader->_windowsSize);

    printf("Data index: %d\r\n", tcpHeader->_dataIndex);
}

