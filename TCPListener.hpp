#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include "TCPConnection.hpp"

class TCPListener {
private:
    int m_fd;
    uint16_t m_port;
    int m_max_pending;

public:
    TCPListener();
    TCPListener(const TCPListener&) = delete;

    bool listen();
    TCPConnection accept();
    bool disconnect();

};

#endif // TCPLISTENER_H
