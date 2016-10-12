#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <cstring>


class TCPconnection
{
    private:
       int socket_desc, client_sock;

    public:
        bool connect();
        bool accept();
        std::string read(int dataSize);
        bool write(const std::string &message);
        bool disconnect();
        TCPconnection();
};

#endif // TCPCONNECTION_H
