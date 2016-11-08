#include "TCPConnection.hpp"

#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace arn {
namespace net {

TCPConnection::TCPConnection(int fd, struct sockaddr_in addr) :
    m_fd(fd),
    m_addr(addr) {
}

TCPConnection::TCPConnection(TCPConnection &&c) {
    std::swap(m_fd, c.m_fd);
    std::swap(m_addr, c.m_addr);
}

TCPConnection& TCPConnection::operator=(TCPConnection &&c) {
    std::swap(m_fd, c.m_fd);
    std::swap(m_addr, c.m_addr);
    return *this;
}

Port TCPConnection::port() const {
  return m_addr.sin_port;
}

std::string TCPConnection::address() const {
  std::ostringstream oss;
  const uint8_t *addr = reinterpret_cast<const uint8_t*>(&m_addr.sin_addr.s_addr);
  oss <<
      static_cast<int>(addr[0]) << '.' <<
      static_cast<int>(addr[1]) << '.' <<
      static_cast<int>(addr[2]) << '.' <<
      static_cast<int>(addr[3]);
  return oss.str();
}

string TCPConnection::read(int dataSize) {
    std::string client_message(dataSize, '\0');
    int read_size, read_total = 0;
    while ( (read_size = ::recv(m_fd, &client_message[read_total] , dataSize, 0)) > 0 ) {
        read_total+=read_size;
        dataSize -= read_size;
    }
    return client_message;
}

std::string TCPConnection::readNonblock(int max_size) {
    std::string client_message(max_size, '\0');
    int res = ::recv(m_fd, &client_message[0], max_size, MSG_DONTWAIT);
    if (res == -1) {
        throw std::runtime_error("recv(2) returned -1: "s + strerror(errno));
    }
    client_message.resize(res);
    return client_message;
}

bool TCPConnection::write(const std::string &message) {
    int wr = ::send(m_fd, message.c_str(), message.length(), 0);
    if (wr == -1){
        cout << "Error while sending data" << endl;
        return false;
    }
    return true;
}

bool TCPConnection::disconnect() {
    close(m_fd);
    return true;
}

}
}
