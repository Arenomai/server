
#include "TCPListener.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/tcp.h>
#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace arn {
namespace net {

TCPListener::TCPListener() :
    m_fd(-1),
    m_port(4242),
    m_max_pending(3) {
}

bool TCPListener::listen() {
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        throw std::runtime_error("socket(2) returned " + std::to_string(socket_desc) + ": " +
            strerror(errno));
    }

    int value = 1;
    int setsockopt_ret = setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    if (setsockopt_ret < 0) {
        throw std::runtime_error("setsockopt(SO_REUSEADDR) returned " + std::to_string(setsockopt_ret) + ": " +
            strerror(errno));
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(m_port);
    int bind_ret = ::bind(socket_desc, reinterpret_cast<struct sockaddr*>(&server), sizeof(server));
    if (bind_ret < 0) {
        throw std::runtime_error("bind(2) returned " + std::to_string(bind_ret) + ": " +
            strerror(errno));
    }
    m_fd = socket_desc;
    ::listen(m_fd, m_max_pending);
    return true;
}

TCPConnection TCPListener::accept() {
    struct sockaddr_in client;
    int c = sizeof(client);
    int client_fd = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&client),
        reinterpret_cast<socklen_t*>(&c));
    if (client_fd < 0) {
        throw std::runtime_error("accept(2) returned " + std::to_string(client_fd) + ": " +
            strerror(errno));
    }
    const int one = 1;
    setsockopt(client_fd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
    return TCPConnection(client_fd, client);
}

TCPListener::~TCPListener() {
    disconnect();
}

bool TCPListener::disconnect() {
    ::close(m_fd);
    return true;
}

}
}
