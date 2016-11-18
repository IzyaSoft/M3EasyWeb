#ifndef DEMOHEADER_H
#define DEMOHEADER_H

const unsigned char demoResponseHeader[] =         // 1st thing our server sends to a client
{
    "HTTP/1.0 200 OK\r\n"                          // protocol ver 1.0, code 200, reason OK
    "Content-Type: text/html\r\n"                  // type of data we want to send
    "\r\n"                                         // indicate end of HTTP-header
};

#endif
