#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <cstring>


class TCPConnection {
private:
    int m_fd;

public:
    TCPConnection(int fd);

    bool connected() const {
        return m_fd >= 0;
    }
    std::string read(int dataSize);
    bool write(const std::string &message);
    bool disconnect();

};

#endif // TCPCONNECTION_H
