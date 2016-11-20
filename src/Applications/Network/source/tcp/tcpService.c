#include "hal.h"
#include "arpCache.h"
#include "ip.h"
#include "tcpService.h"
#include "networkService.h"

extern struct NetworkApplicationConfig* networkApplicationsConfig [];
extern unsigned short numberOfConfigs;
static uint32_t sequenceNumberUpperWord = 0;
static uint32_t tcpServiceClock = 0;

static unsigned char HandleApplicationTcpState(struct NetworkApplicationConfig* application, struct TcpHeader* tcpHeader, struct EthernetBuffer* buffer);
static struct NetworkApplicationConfig* Filtrate(struct TcpHeader* tcpHeader);
static void ReloadRetransmissionClocks(struct NetworkApplicationConfig* application);
static void StartTimer(struct NetworkApplicationConfig* application);    // todo: umv: very strange function, should be re-factored
static void StopTimer(struct NetworkApplicationConfig* application);     // todo: umv: very strange function, should be re-factored
static void RestartTimer(struct NetworkApplicationConfig* application);  // todo: umv: very strange function, should be re-factored
static void HandleLastTcpPacketAckowledgedState(struct NetworkApplicationConfig* application, struct EthernetBuffer* buffer);
// todo: umv: very strange functions (all below this linSettingPlae), should be re-factored
static void  HandleFinalTcpState(struct NetworkApplicationConfig* application, struct EthernetBuffer* buffer);
static void FinalizeTcpOperations(struct NetworkApplicationConfig* application, struct EthernetBuffer* buffer);
static void HandleTimeout(struct NetworkApplicationConfig* application);


void HandleTcpServiceClockTick()
{
    sequenceNumberUpperWord++;
    tcpServiceClock++;
}

unsigned char HandleTcpPacket(struct EthernetBuffer* buffer)
{
    struct TcpHeader tcpHeader= ReadTcpHeader(buffer);
    struct NetworkApplicationConfig* selectedApplication = Filtrate(&tcpHeader);
    if(selectedApplication != 0)
    {
        unsigned char result = HandleApplicationTcpState(selectedApplication, &tcpHeader, buffer);
        FinalizeTcpOperations(selectedApplication, buffer);
        return result;
    }
    return 0;
}

void SendTcpData(struct NetworkApplicationConfig* application, struct EthernetBuffer* txBuffer, unsigned short tcpDataLength)
{
    //if(application->_socketStatus & SOCK_TX_BUF_RELEASED)
    //{
        application->_socketStatus &= ~SOCK_TX_BUF_RELEASED;                      // occupy tx-buffer
        application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber += tcpDataLength;     // advance UNA
        //application->_client[0]._handshakeInfo._sequenceNumber += tcpDataLength;       // todo: umv: custom SHIT, hack (wrong) // we shoul received ack here ! or retransmit buofer
        StartTimer(application);
        TransmitData(txBuffer);
    //}
}

static void ReloadRetransmissionClocks(struct NetworkApplicationConfig* application)
{
    tcpServiceClock = 0;
    application->_retryCounter = MAX_RETRIES;
    application->_tcpFlags|= TCP_TIMER_RUNNING;
    application->_tcpFlags |= TIMER_TYPE_RETRY;
}

// todo: umv: very strange function, should be re-factored
static void StartTimer(struct NetworkApplicationConfig* application)
{
    tcpServiceClock = 0;
    application->_tcpFlags |= TCP_TIMER_RUNNING;
    application->_tcpFlags &= ~TIMER_TYPE_RETRY;
}

// todo: umv: very strange function, should be re-factored
static void StopTimer(struct NetworkApplicationConfig* application)
{
    application->_tcpFlags &= ~TCP_TIMER_RUNNING;
}

// todo: umv: very strange function, should be re-factored
static void RestartTimer(struct NetworkApplicationConfig* application)
{
    tcpServiceClock = 0;
}

static struct NetworkApplicationConfig* Filtrate(struct TcpHeader* tcpHeader)
{
    for(unsigned char appCounter= 0; appCounter < numberOfConfigs; appCounter++)
    {
        if(networkApplicationsConfig[appCounter]->_isTcpApplication && networkApplicationsConfig[appCounter]->_applicationPort == tcpHeader->_destinationPort)
        return networkApplicationsConfig[appCounter];
    }
    return 0;
}

static unsigned char HandleApplicationTcpState(struct NetworkApplicationConfig* application, struct TcpHeader* tcpHeader, struct EthernetBuffer* buffer)
{
    char fail = -1;
    unsigned char result = 0;
    unsigned char tcpCode = 0;
    switch(application->_tcpState)
    {
        case CLOSED:
             if(!(tcpHeader->_flags & TCP_CODE_RST))
             {
                 //printf ("closed case \r\n");
                 memcpy(application->_client[0]._ipAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
                 memcpy(application->_client[0]._macAddress, &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
                 application->_client[0]._tcpPort = tcpHeader->_sourcePort;
                 if(CheckEntryIsPresent(application->_client[0]._ipAddress)  == fail)
                 {

                    struct ArpEntry entry;
                    memcpy(entry._ipAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
                    memcpy(entry._macAddress, &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
                    AddEntry(&entry, tcpServiceClock / 10);
                    SendArpRequest(application->_client[0]._ipAddress);
                    //break;
                 }
                 //printf ("in closed state\r\n");

                 if(tcpHeader->_flags & TCP_CODE_ACK)
                 {
                     application->_client[0]._handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                     tcpCode = TCP_CODE_RST;
                 }
                 else
                 {
                     application->_client[0]._handshakeInfo._sequenceNumber = 0;
                     application->_client[0]._handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber + GetWord(buffer, IP_PACKET_SIZE_INDEX) - IP_HEADER_SIZE - tcpHeader->_headerSize;
                     if(tcpHeader->_flags & (TCP_CODE_SYN | TCP_CODE_FIN))
                         application->_client[0]._handshakeInfo._acknowledgementNumber++;
                     tcpCode = TCP_CODE_RST | TCP_CODE_ACK;
                 }
                 //printf ("sending response from closed state \r\n");
                 BuildTcpFrame(buffer, tcpCode, application->_applicationPort, &application->_client[0]);
                 TransmitData(buffer);
             }
             break;
        case LISTENING:
             //printf ("in listening state\r\n");
             if(!(tcpHeader->_flags & TCP_CODE_RST))
             {
                 memcpy(application->_client[0]._ipAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
                 memcpy(application->_client[0]._macAddress, &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
                 application->_client[0]._tcpPort = tcpHeader->_sourcePort;
                 if(CheckEntryIsPresent(application->_client[0]._ipAddress)  == fail)
                 {
                     SendArpRequest(application->_client[0]._ipAddress);
                     struct ArpEntry entry;
                     memcpy(entry._ipAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH);
                     memcpy(entry._macAddress, &buffer->_buffer[ETHERNET_SOURCE_ADDRESS_INDEX], MAC_ADDRESS_LENGTH);
                     AddEntry(&entry, tcpServiceClock / 10);
                     //break;
                 }
                 if(tcpHeader->_flags & TCP_CODE_ACK)
                 {
                     application->_client[0]._handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                     tcpCode = TCP_CODE_RST;
                 }
                 else if(tcpHeader->_flags & TCP_CODE_SYN)
                 {
                     tcpCode = TCP_CODE_SYN | TCP_CODE_ACK;
                     application->_client[0]._handshakeInfo._acknowledgementNumber =  tcpHeader->_sequenceNumber + 1;
                     application->_client[0]._handshakeInfo._sequenceNumber = ((unsigned long)sequenceNumberUpperWord << 16) | (GetTimerCountValue(0) & 0xFFFF);
                     application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber = application->_client[0]._handshakeInfo._sequenceNumber + 1;
                     ReloadRetransmissionClocks(application);
                     application->_tcpState = SYN_RECD;
                 }
                 else break;
                 BuildTcpFrame(buffer, tcpCode, application->_applicationPort, &application->_client[0]);
                 TransmitData(buffer);
             }
             break;
        case SYN_SENT:
             if(memcmp(application->_client[0]._ipAddress, &buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH))
                 break;
             if(tcpHeader->_flags & TCP_CODE_ACK)
             {
                 if(tcpHeader->_acknowledgementNumber != application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber)
                 {
                     if(!(tcpHeader->_flags & TCP_CODE_RST))
                     {
                         application->_client[0]._handshakeInfo._sequenceNumber = tcpHeader->_acknowledgementNumber;
                         BuildTcpFrame(buffer, TCP_CODE_RST, application->_applicationPort, &application->_client[0]);
                         TransmitData(buffer);
                     }
                 }
             }
             if(tcpHeader->_flags& TCP_CODE_RST)
             {
                 if (tcpHeader->_flags & TCP_CODE_ACK)              // if ACK was acceptable, reset connection
                 {
                     application->_tcpState = CLOSED;
                     application->_tcpFlags = 0;                          // reset all flags, stop retransmission...
                     application->_socketStatus = SOCK_ERR_CONN_RESET;
                 }
                 break;
             }
             if(tcpHeader->_flags & TCP_CODE_SYN)
             {
                 application->_client[0]._handshakeInfo._acknowledgementNumber = tcpHeader->_sequenceNumber + 1;
                 StopTimer(application);
                 if(tcpHeader->_flags & TCP_CODE_ACK)
                 {
                                            // stop retransmission, other TCP got our SYN
                     application->_client[0]._handshakeInfo._sequenceNumber = application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber;                // advance our sequence number
                     tcpCode = TCP_CODE_ACK;        // ACK this ISN
                     application->_tcpState = ESTABLISHED;
                     application->_socketStatus |= SOCK_CONNECTED;
                     application->_socketStatus |= SOCK_TX_BUF_RELEASED;  // user may send data now :-)
                 }
                 else
                 {
                     tcpCode = TCP_CODE_SYN | TCP_CODE_ACK;
                     ReloadRetransmissionClocks(application);
                     application->_tcpState = SYN_RECD;
                 }
                 BuildTcpFrame(buffer, TCP_CODE_RST, application->_applicationPort, &application->_client[0]);
                 TransmitData(buffer);
             }
             break;
        default:
             if(memcmp(application->_client[0]._ipAddress, &buffer->_buffer[IP_PACKET_HEADER_SOURCE_IP_INDEX], IPV4_LENGTH))
                break;
             if (tcpHeader->_sequenceNumber != application->_client[0]._handshakeInfo._acknowledgementNumber) // drop if it's not the segment we expect
                break;
             if(tcpHeader->_flags & TCP_CODE_RST)
             {
                 application->_tcpState = CLOSED;                // close the state machine
                 application->_tcpFlags = 0;                            // reset all flags, stop retransmission...
                 application->_socketStatus = SOCK_ERR_CONN_RESET;      // indicate an error to user
                 break;
             }
             if(tcpHeader->_flags & TCP_CODE_SYN)
             {
                 BuildTcpFrame(buffer, TCP_CODE_RST, application->_applicationPort, &application->_client[0]);
                 TransmitData(buffer);
                 application->_tcpState = CLOSED;                // close connection...
                 application->_tcpFlags = 0;                            // reset all flags, stop retransmission...
                 application->_socketStatus = SOCK_ERR_REMOTE;          // fatal error!
                 break;                                   // ...and drop the frame
             }
             if(!(tcpHeader->_flags & TCP_CODE_ACK))
                 break;
             if(tcpHeader->_acknowledgementNumber == application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber)
             {
                 StopTimer(application);                          // stop retransmission
                 application->_client[0]._handshakeInfo._sequenceNumber = application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber;                  // advance our sequence number
                 HandleLastTcpPacketAckowledgedState(application, buffer);
                 if(application->_tcpState == ESTABLISHED)      // if true, give the frame buffer back
                     application->_socketStatus |= SOCK_TX_BUF_RELEASED;  // to user
                 if ((application->_tcpState == ESTABLISHED) || (application->_tcpState == FIN_WAIT_1) || (application->_tcpState == FIN_WAIT_2))
                 {
                     if(GetWord(buffer, IP_PACKET_SIZE_INDEX))
                     {
                         result = 1;
                         //TCPRxDataCount = NrOfDataBytes;                // ...tell the user...
                         application->_socketStatus |= SOCK_DATA_AVAILABLE;           // indicate the new data to user
                         application->_client[0]._handshakeInfo._acknowledgementNumber += GetWord(buffer, IP_PACKET_SIZE_INDEX);
                         BuildTcpFrame(buffer, TCP_CODE_ACK, application->_applicationPort, &application->_client[0]);
                         TransmitData(buffer);
                     }
                 }
                 if(tcpHeader->_flags & TCP_CODE_FIN)
                     HandleFinalTcpState(application, buffer);
             }
             break;
    }
    return result;
}

static void HandleLastTcpPacketAckowledgedState(struct NetworkApplicationConfig* application, struct EthernetBuffer* buffer)
{
    switch(application->_tcpState)
    {
        case SYN_RECD:
             application->_tcpState = ESTABLISHED;       // user may send data now :-)
             application->_socketStatus |= SOCK_CONNECTED;
             break;
             break;
        case FIN_WAIT_1:
             application->_tcpState = FIN_WAIT_2;
             break;
        case CLOSING:
             application->_tcpState = TIME_WAIT;
             break;
        case LAST_ACK:
             application->_tcpState = CLOSED;
             application->_tcpFlags = 0;                        // reset all flags, stop retransmission...
             application->_socketStatus &= SOCK_DATA_AVAILABLE; // clear all flags but data available
             break;
        case TIME_WAIT:
             BuildTcpFrame(buffer, TCP_CODE_ACK, application->_applicationPort, &application->_client[0]);
             TransmitData(buffer);
             RestartTimer(application);                   // restart TIME_WAIT timeout
             break;
        default:
             break;
    }
}

static void HandleFinalTcpState(struct NetworkApplicationConfig* application, struct EthernetBuffer* buffer)
{
    switch (application->_tcpState)
    {
        case SYN_RECD :
        case ESTABLISHED :
             application->_tcpState = CLOSE_WAIT;
             break;
        case FIN_WAIT_1: // if our FIN was ACKed, we automatically
             application->_tcpState = CLOSING;           // enter FIN_WAIT_2 (look above) and therefore
             application->_socketStatus &= ~SOCK_CONNECTED;     // TIME_WAIT
             break;
        case FIN_WAIT_2:
             StartTimer(application);
             application->_tcpState = TIME_WAIT;
             application->_socketStatus &= ~SOCK_CONNECTED;
             break;
        case TIME_WAIT :
             RestartTimer(application);
             break;
        default:
             break;
    }
    application->_client[0]._handshakeInfo._acknowledgementNumber++;                              // ACK remote's FIN flag
    BuildTcpFrame(buffer, TCP_CODE_ACK, application->_applicationPort, &application->_client[0]);
    TransmitData(buffer);
}

static void FinalizeTcpOperations(struct NetworkApplicationConfig* application, struct EthernetBuffer* buffer)
{
    if(application->_tcpFlags & TCP_TIMER_RUNNING)
    {
        if (application->_tcpFlags & TIMER_TYPE_RETRY)
        {
            if (tcpServiceClock > RETRY_TIMEOUT)
            {
                RestartTimer(application);                       // set a new timeout
                if(application->_retryCounter)
                {
                    //TCPHandleRetransmission();             // resend last frame
                    application->_retryCounter--;
                }
                else
                {
                    StopTimer(application);
                    HandleTimeout(application);
                }
            }
        }
        else if (tcpServiceClock > FIN_TIMEOUT)
        {
            application->_tcpState = CLOSED;
            application->_tcpFlags = 0;                              // reset all flags, stop retransmission...
            application->_socketStatus &= SOCK_DATA_AVAILABLE;       // clear all flags but data available
        }
    }
    switch (application->_tcpState)
    {
        case CLOSED:
        case LISTENING:
             if(application->_tcpFlags & TCP_ACTIVE_OPEN)            // stack has to open a connection?
             {
                 application->_client[0]._handshakeInfo._sequenceNumber = ((unsigned long)sequenceNumberUpperWord << 16) | (GetTimerCountValue(0) & 0xFFFF);
                 application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber = application->_client[0]._handshakeInfo._sequenceNumber;
                 application->_client[0]._handshakeInfo._acknowledgementNumber = 0;                                       // we don't know what to ACK!
                 application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber++;                                      // count SYN as a byte
                 BuildTcpFrame(buffer, TCP_CODE_SYN, application->_applicationPort, &application->_client[0]);
                 TransmitData(buffer);
                 StartTimer(application);                               // we NEED a retry-timeout
                 application->_tcpState = SYN_SENT;
              }
              break;
         case SYN_RECD:
         case ESTABLISHED:
              if(application->_tcpFlags & TCP_CLOSE_REQUESTED)                  // user has user initated a close?
              {
                  if(application->_client[0]._handshakeInfo._sequenceNumber == application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber)                          // all data ACKed?
                  {
                      application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber++;
                      BuildTcpFrame(buffer, TCP_CODE_FIN | TCP_CODE_ACK, application->_applicationPort, &application->_client[0]);
                      TransmitData(buffer);
                      StartTimer(application);
                      application->_tcpState = FIN_WAIT_1;
                  }
              }
              break;
         case CLOSE_WAIT:
              if(application->_client[0]._handshakeInfo._sequenceNumber == application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber)                            // all data ACKed?
              {
                  application->_client[0]._handshakeInfo._unAcknowledgedSequenceNumber++;                                        // count FIN as a byte
                  BuildTcpFrame(buffer, TCP_CODE_FIN | TCP_CODE_ACK, application->_applicationPort, &application->_client[0]);
                  TransmitData(buffer);
                  StartTimer(application);
                  application->_tcpState = LAST_ACK;
              }
              break;
         default:
              break;
    }
}

static void HandleTimeout(struct NetworkApplicationConfig* application)
{
    application->_tcpState = CLOSED;
    if (application->_tcpFlags & (TCP_ACTIVE_OPEN))// | IP_ADDR_RESOLVED)) == TCP_ACTIVE_OPEN)
        application->_socketStatus = SOCK_ERR_ARP_TIMEOUT;         // indicate an error to user
    else
        application->_socketStatus = SOCK_ERR_TCP_TIMEOUT;

    application->_tcpFlags = 0;
}
