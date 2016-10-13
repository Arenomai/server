#ifndef TCPLISTENER_H
#define TCPLISTENER_H
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <cstring>

#include "TCPConnection.hpp"

class TCPListener {
private:
    int socket_desc;
    int m_port;
    int m_maxPending;

public:
    TCPListener();

    bool connect();
    TCPConnection accept();
    bool disconnect();

};

#endif // TCPLISTENER_H
