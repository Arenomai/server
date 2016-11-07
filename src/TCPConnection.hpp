#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <arpa/inet.h>
#include <string>

class TCPConnection {
private:
    int m_fd;
    struct sockaddr_in m_addr;

public:
    TCPConnection(int fd, struct sockaddr_in addr);
    TCPConnection(const TCPConnection&) = delete;
    TCPConnection(TCPConnection&&);

    TCPConnection& operator=(const TCPConnection&) = delete;
    TCPConnection& operator=(TCPConnection&&);

    bool connected() const {
        return m_fd >= 0;
    }

    uint16_t port() const;
    std::string address() const;

    std::string read(int dataSize);
    bool write(const std::string &message);
    bool disconnect();

};

#endif // TCPCONNECTION_H
