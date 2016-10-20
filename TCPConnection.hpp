#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>

class TCPConnection {
private:
    int m_fd;

public:
    TCPConnection(int fd);
    TCPConnection(const TCPConnection&) = delete;
    TCPConnection(TCPConnection&&);

    TCPConnection& operator=(const TCPConnection&) = delete;
    TCPConnection& operator=(TCPConnection&&);

    bool connected() const {
        return m_fd >= 0;
    }
    std::string read(int dataSize);
    bool write(const std::string &message);
    bool disconnect();

};

#endif // TCPCONNECTION_H
