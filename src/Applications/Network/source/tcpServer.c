#include "tcpServer.h"
#include "tcp.h"

//TcpState tcpStateMachine;

void HandleTcpPacket(struct EthernetBuffer* buffer)
{
    struct TcpHeader tcpHeader;
    ReadTcpHeader(buffer, &tcpHeader);
}

/*	  switch (TCPStateMachine)                                 // implement the TCP state machine
	  {
	    case CLOSED :
	    {
	      if (!(TCPCode & TCP_CODE_RST))
	      {
	        TCPRemotePort = TCPSegSourcePort;
	        memcpy(&RemoteMAC, &RecdFrameMAC, 6);              // save opponents MAC and IP
	        memcpy(&RemoteIP, &RecdFrameIP, 4);                // for later use

	        if (TCPCode & TCP_CODE_ACK)                        // make the reset sequence
	        {                                                  // acceptable to the other
	          TCPSeqNr = TCPSegAck;                            // TCP
	          PrepareTCP_FRAME(TCP_CODE_RST);
	        }
	        else
	        {
	          TCPSeqNr = 0;
	          TCPAckNr = TCPSegSeq + NrOfDataBytes;
	          if (TCPCode & (TCP_CODE_SYN | TCP_CODE_FIN)) TCPAckNr++;
	          PrepareTCP_FRAME(TCP_CODE_RST | TCP_CODE_ACK);
	        }
	      }
	      break;
	    }
	    case LISTENING :
	    {
	      if (!(TCPCode & TCP_CODE_RST))                       // ignore segment containing RST
	      {
	        TCPRemotePort = TCPSegSourcePort;
	        memcpy(&RemoteMAC, &RecdFrameMAC, 6);              // save opponents MAC and IP
	        memcpy(&RemoteIP, &RecdFrameIP, 4);                // for later use

	        if (TCPCode & TCP_CODE_ACK)                        // reset a bad
	        {                                                  // acknowledgement
	          TCPSeqNr = TCPSegAck;
	          PrepareTCP_FRAME(TCP_CODE_RST);
	        }
	        else if (TCPCode & TCP_CODE_SYN)
	        {
	          TCPAckNr = TCPSegSeq + 1;                           // get remote ISN, next byte we expect
	//            TCPSeqNr = ((unsigned long)ISNGenHigh << 16) | (T0TC & 0xFFFF);  // Keil: changed from TAR to T0TC;
	            TCPSeqNr = ((unsigned long)ISNGenHigh << 16) | (LPC_TIM0->TC & 0xFFFF);  // Keil: changed from TAR to T0TC;
	                                                              // set local ISN
	          TCPUNASeqNr = TCPSeqNr + 1;                         // one byte out -> increase by one
	          PrepareTCP_FRAME(TCP_CODE_SYN | TCP_CODE_ACK);
	          LastFrameSent = TCP_SYN_ACK_FRAME;
	          TCPStartRetryTimer();
	          TCPStateMachine = SYN_RECD;
	        }
	      }
	      break;
	    }
	    case SYN_SENT :
	    {
	      if (memcmp(&RemoteIP, &RecdFrameIP, 4)) break;  // drop segment if its IP doesn't belong
	                                                      // to current session

	      if (TCPSegSourcePort != TCPRemotePort) break;   // drop segment if port doesn't match

	      if (TCPCode & TCP_CODE_ACK)                // ACK field significant?
	        if (TCPSegAck != TCPUNASeqNr)            // is our ISN ACKed?
	        {
	          if (!(TCPCode & TCP_CODE_RST))
	          {
	            TCPSeqNr = TCPSegAck;
	            PrepareTCP_FRAME(TCP_CODE_RST);
	          }
	          break;                                 // drop segment
	        }

	      if (TCPCode & TCP_CODE_RST)                // RST??
	      {
	        if (TCPCode & TCP_CODE_ACK)              // if ACK was acceptable, reset
	        {                                        // connection
	          TCPStateMachine = CLOSED;
	          TCPFlags = 0;                          // reset all flags, stop retransmission...
	          SocketStatus = SOCK_ERR_CONN_RESET;
	        }
	        break;                                   // drop segment
	      }

	      if (TCPCode & TCP_CODE_SYN)                // SYN??
	      {
	        TCPAckNr = TCPSegSeq;                    // get opponents ISN
	        TCPAckNr++;                              // inc. by one...

	        if (TCPCode & TCP_CODE_ACK)
	        {
	          TCPStopTimer();                        // stop retransmission, other TCP got our SYN
	          TCPSeqNr = TCPUNASeqNr;                // advance our sequence number

	          PrepareTCP_FRAME(TCP_CODE_ACK);        // ACK this ISN
	          TCPStateMachine = ESTABLISHED;
	          SocketStatus |= SOCK_CONNECTED;
	          SocketStatus |= SOCK_TX_BUF_RELEASED;  // user may send data now :-)
	        }
	        else
	        {
	          TCPStopTimer();
	          PrepareTCP_FRAME(TCP_CODE_SYN | TCP_CODE_ACK);   // our SYN isn't ACKed yet,
	          LastFrameSent = TCP_SYN_ACK_FRAME;               // now continue with sending
	          TCPStartRetryTimer();                            // SYN_ACK frames
	          TCPStateMachine = SYN_RECD;
	        }
	      }
	      break;
	    }
	    default :
	    {
	      if (memcmp(&RemoteIP, &RecdFrameIP, 4)) break;  // drop segment if IP doesn't belong
	                                                      // to current session

	      if (TCPSegSourcePort != TCPRemotePort) break;   // drop segment if port doesn't match

	      if (TCPSegSeq != TCPAckNr) break;               // drop if it's not the segment we expect

	      if (TCPCode & TCP_CODE_RST)                // RST??
	      {
	        TCPStateMachine = CLOSED;                // close the state machine
	        TCPFlags = 0;                            // reset all flags, stop retransmission...
	        SocketStatus = SOCK_ERR_CONN_RESET;      // indicate an error to user
	        break;
	      }

	      if (TCPCode & TCP_CODE_SYN)                // SYN??
	      {
	        PrepareTCP_FRAME(TCP_CODE_RST);          // is NOT allowed here! send a reset,
	        TCPStateMachine = CLOSED;                // close connection...
	        TCPFlags = 0;                            // reset all flags, stop retransmission...
	        SocketStatus = SOCK_ERR_REMOTE;          // fatal error!
	        break;                                   // ...and drop the frame
	      }

	      if (!(TCPCode & TCP_CODE_ACK)) break;      // drop segment if the ACK bit is off

	      if (TCPSegAck == TCPUNASeqNr)              // is our last data sent ACKed?
	      {
	        TCPStopTimer();                          // stop retransmission
	        TCPSeqNr = TCPUNASeqNr;                  // advance our sequence number

	        switch (TCPStateMachine)                 // change state if necessary
	        {
	          case SYN_RECD :                        // ACK of our SYN?
	          {
	            TCPStateMachine = ESTABLISHED;       // user may send data now :-)
	            SocketStatus |= SOCK_CONNECTED;
	            break;
	          }
	          case FIN_WAIT_1 : { TCPStateMachine = FIN_WAIT_2; break; } // ACK of our FIN?
	          case CLOSING :    { TCPStateMachine = TIME_WAIT; break; }  // ACK of our FIN?
	          case LAST_ACK :                                            // ACK of our FIN?
	          {
	            TCPStateMachine = CLOSED;
	            TCPFlags = 0;                        // reset all flags, stop retransmission...
	            SocketStatus &= SOCK_DATA_AVAILABLE; // clear all flags but data available
	            break;
	          }
	          case TIME_WAIT :
	          {
	            PrepareTCP_FRAME(TCP_CODE_ACK);      // ACK a retransmission of remote FIN
	            TCPRestartTimer();                   // restart TIME_WAIT timeout
	            break;
	          }
	        }

	        if (TCPStateMachine == ESTABLISHED)      // if true, give the frame buffer back
	          SocketStatus |= SOCK_TX_BUF_RELEASED;  // to user
	      }

	      if ((TCPStateMachine == ESTABLISHED) || (TCPStateMachine == FIN_WAIT_1) || (TCPStateMachine == FIN_WAIT_2))
	        if (NrOfDataBytes)                                 // data available?
	          if (!(SocketStatus & SOCK_DATA_AVAILABLE))       // rx data-buffer empty?
	          {
	            DummyReadFrame_EMAC(6);                        // ignore window, checksum, urgent pointer
	            CopyFromFrame_EMAC(RxTCPBuffer, NrOfDataBytes);// fetch data and
	            TCPRxDataCount = NrOfDataBytes;                // ...tell the user...
	            SocketStatus |= SOCK_DATA_AVAILABLE;           // indicate the new data to user
	            TCPAckNr += NrOfDataBytes;
	            PrepareTCP_FRAME(TCP_CODE_ACK);                // ACK rec'd data
	          }

	      if (TCPCode & TCP_CODE_FIN)                // FIN??
	      {
	        switch (TCPStateMachine)
	        {
	          case SYN_RECD :
	          case ESTABLISHED :
	          {
	            TCPStateMachine = CLOSE_WAIT;
	            break;
	          }
	          case FIN_WAIT_1 :
	          {                                      // if our FIN was ACKed, we automatically
	            TCPStateMachine = CLOSING;           // enter FIN_WAIT_2 (look above) and therefore
	            SocketStatus &= ~SOCK_CONNECTED;     // TIME_WAIT
	            break;
	          }
	          case FIN_WAIT_2 :
	          {
	            TCPStartTimeWaitTimer();
	            TCPStateMachine = TIME_WAIT;
	            SocketStatus &= ~SOCK_CONNECTED;
	            break;
	          }
	          case TIME_WAIT :
	          {
	            TCPRestartTimer();
	            break;
	          }
	        }
	        TCPAckNr++;                              // ACK remote's FIN flag
	        PrepareTCP_FRAME(TCP_CODE_ACK);
	      }
	    }
	  }*/
