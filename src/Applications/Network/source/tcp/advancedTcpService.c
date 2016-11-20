#include "hal.h"
#include "arpCache.h"
#include "ip.h"
#include "networkService.h"
#include "advancedTcpService.h"

extern struct NetworkApplicationConfig* networkApplicationsConfig [];
extern unsigned short numberOfConfigs;
static uint32_t sequenceNumberUpperWord = 0;
static uint32_t tcpServiceClock = 0;
struct EthernetBuffer txBuffer;
extern unsigned char localBuffer[SMALL_FRAME_SIZE];

static struct NetworkApplicationConfig* GetApplication(struct TcpHeader* tcpHeader);
static struct NetworkApplicationClient* GetClient(struct NetworkApplicationConfig* application, unsigned char* macAddress, unsigned short clientPort);
static void ProcessTcpClient(struct NetworkApplicationClient* client, struct TcpHeader* tcpHeader, struct EthernetBuffer* rxBuffer);
static void TransmitTcpFrame(struct NetworkApplicationClient* client, unsigned short applicationPort, unsigned short tcpCode, unsigned char useRetransmission);

void ProcessTcpPacket(struct EthernetBuffer* rxBuffer)
{
    struct TcpHeader tcpHeader= ReadTcpHeader(rxBuffer);
    struct NetworkApplicationConfig* selectedApplication = GetApplication(&tcpHeader);
    if(selectedApplication != 0)
    {
        // Add / update arp cache
        struct ArpEntry entry;
        memcpy(entry._ipAddress, &rxBuffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
        memcpy(entry._macAddress, &rxBuffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
        AddEntry(&entry, tcpServiceClock / 10);
        SendArpRequest(entry._ipAddress);
        struct NetworkApplicationClient* client = GetClient(selectedApplication, entry._macAddress, tcpHeader._sourcePort);
        if(client != 0)
        {
            memcpy(client->_ipAddress, &rxBuffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
            memcpy(client->_macAddress, &rxBuffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
            client->_tcpPort = tcpHeader._sourcePort;
            ProcessTcpClient(client, &tcpHeader, rxBuffer);
        }
    }
}

static void ProcessTcpClient(struct NetworkApplicationClient* client, struct TcpHeader* tcpHeader, struct EthernetBuffer* rxBuffer)
{
    unsigned char tcpCode;
    txBuffer._buffer = localBuffer;
    txBuffer._bufferCapacity = SMALL_FRAME_SIZE;
    unsigned char sendBack = 0;
    unsigned char nextState;
    if(tcpHeader->_flags & TCP_CODE_RST)
    {
        client->_tcpState = CLOSED;
        client->_socketStatus = SOCKET_CLOSED;
    }
    switch(client->_tcpState)
    {
         case CLOSED:
              if(tcpHeader->_flags & TCP_CODE_ACK)
              {
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  tcpCode = TCP_CODE_RST;
              }
              else
              {
                  client->_handshakeInfo._sequenceNumber = 0;
                  client->_handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber + GetWord(&txBuffer, IP_PACKET_SIZE_INDEX) - IP_HEADER_SIZE - tcpHeader->_headerSize;
                  if(tcpHeader->_flags & (TCP_CODE_SYN | TCP_CODE_FIN))
                      client->_handshakeInfo._acknowledgementNumber++;
                  tcpCode = TCP_CODE_RST | TCP_CODE_ACK;
               }
               sendBack = 1;
               nextState = LISTENING;
              break;
         case LISTENING:
              if(tcpHeader->_flags & TCP_CODE_ACK)
              {
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  tcpCode = TCP_CODE_RST;
              }
              else if(tcpHeader->_flags & TCP_CODE_SYN)
              {
                  tcpCode = TCP_CODE_SYN | TCP_CODE_ACK;
                  client->_handshakeInfo._acknowledgementNumber =  tcpHeader->_sequenceNumber + 1;
                  client->_handshakeInfo._sequenceNumber = ((unsigned long)sequenceNumberUpperWord << 16) | (GetTimerCountValue(0) & 0xFFFF);
                  client->_handshakeInfo._unAcknowledgedSequenceNumber = client->_handshakeInfo._sequenceNumber + 1;
                  nextState = SYN_RECD;
                  client->_socketStatus = SOCKET_OPENED;
              }
              else break;
              sendBack = 1;
              break;
         case SYN_RECD:
              if(tcpHeader->_flags & TCP_CODE_ACK)
              {
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  nextState = ESTABLISHED;
                  client->_socketStatus = SOCKET_CONNECTED;
              }
              break;
         case ESTABLISHED:
              if(tcpHeader->_flags & TCP_CODE_FIN)
              {
                  tcpCode = TCP_CODE_ACK;
                  sendBack = 1;
                  nextState = CLOSE_WAIT;
                  client->_socketStatus = SOCKET_CLOSING;
              }
              else
              {
                  unsigned short payload = GetWord(rxBuffer, IP_PACKET_SIZE_INDEX);
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  client->_handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber + payload;
                  client->_hasData = 1;
                  client->_dataLength = payload;
                  tcpCode = TCP_CODE_ACK;
                  sendBack = 1;
                  // send FIN if we should drop connect (by timeout or something else)
              }
              break;
         case FIN_WAIT_1:
              if(tcpHeader->_flags & TCP_CODE_FIN)
              {
                  client->_socketStatus = SOCKET_CLOSING;
                  tcpCode = TCP_CODE_ACK;
                  nextState = CLOSING;
                  sendBack = 1;
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  client->_handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber;
              }
              else if(tcpHeader->_flags & TCP_CODE_ACK)
              {
                  client->_socketStatus = SOCKET_CLOSING;
              }
              break;
         case FIN_WAIT_2:
              if(tcpHeader->_flags & TCP_CODE_FIN)
              {
                  tcpCode = TCP_CODE_ACK;
                  nextState = TIME_WAIT;
                  sendBack = 1;
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  client->_handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber;
              }
              break;
         case CLOSE_WAIT:
              if(tcpHeader->_flags & TCP_CODE_FIN)
              {
                  tcpCode = TCP_CODE_FIN;
                  nextState = LAST_ACK;
                  sendBack = 1;
                  client->_handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                  client->_handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber;
              }
              break;
         case TIME_WAIT:
              // todo: umv: wait a bit
              nextState = CLOSED;
              break;
         case LAST_ACK:
              if(tcpHeader->_flags & TCP_CODE_ACK)
              {
                 nextState = CLOSED;
              }
              break;
         default:
              nextState = CLOSED;
              client->_socketStatus = SOCKET_CLOSED;
              break;
    }
    // check client activity on timeout!
    client->_tcpState = nextState;
    if(sendBack)
        TransmitTcpFrame(client, tcpHeader->_destinationPort, tcpCode, 0);
}

static struct NetworkApplicationConfig* GetApplication(struct TcpHeader* tcpHeader)
{
    for(unsigned char appCounter= 0; appCounter < numberOfConfigs; appCounter++)
    {
        if(networkApplicationsConfig[appCounter]->_isTcpApplication && networkApplicationsConfig[appCounter]->_isEnabled &&
           networkApplicationsConfig[appCounter]->_applicationPort == tcpHeader->_destinationPort)
        return networkApplicationsConfig[appCounter];
    }
    return 0;
}

static struct NetworkApplicationClient* GetClient(struct NetworkApplicationConfig* application, unsigned char* macAddress, unsigned short clientPort)
{
    // 1. Search matching client ...
    for(unsigned char clientCounter = 0; clientCounter < MAX_CLIENTS_NUMBER; clientCounter++)
    {
        if(application->_client[clientCounter]._tcpPort == clientPort && CheckMacAddressEquals(application->_client[clientCounter]._macAddress, macAddress))
            return &application->_client[clientCounter];
    }
    // 2. Search free client
    for(unsigned char clientCounter = 0; clientCounter < MAX_CLIENTS_NUMBER; clientCounter++)
    {
        if(application->_client[clientCounter]._socketStatus == SOCKET_CLOSED)
            return &application->_client[clientCounter];
    }
    return 0; // oups, no free clients
}

static void TransmitTcpFrame(struct NetworkApplicationClient* client, unsigned short applicationPort, unsigned short tcpCode, unsigned char useRetransmission)
{
    //todo: handle retransmission
    BuildTcpFrame(&txBuffer, tcpCode, applicationPort, client);
    TransmitData(&txBuffer);
}

void HandleTcpServiceClockTick()
{
    sequenceNumberUpperWord++;
    tcpServiceClock++;
}
