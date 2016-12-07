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

void TCPConnection::write(const std::string &message) {
    int wr = ::send(m_fd, message.c_str(), message.length(), 0);
    if (wr == -1){
        throw std::runtime_error("send(2) returned -1: "s + strerror(errno));
    }
}

bool TCPConnection::read(InMessage &msg) {
    if (m_header_received < Message::HeaderSize) {
        // Receiving header
        const size_t want = Message::HeaderSize - m_header_received;
        int res = ::recv(m_fd, &m_header_buffer, want, MSG_DONTWAIT);
        if (res < 0) {
            throw std::runtime_error("recv(2) returned "s + std::to_string(res) + ": " +
                    strerror(errno));
        }
        m_header_received += res;
        if (m_header_received == Message::HeaderSize) {
            m_msg_size = Message::HeaderSize + m_header_buffer.data_length;
            m_msg_buffer = new byte[m_msg_size];
            m_msg_received = 0;
            ::memcpy(m_msg_buffer, &m_header_buffer, Message::HeaderSize);
            m_msg_received += Message::HeaderSize;
            if (m_header_buffer.data_length == 0) {
                msg.fromData(m_msg_buffer, m_msg_size);
                m_msg_buffer = nullptr;
                m_header_received = 0;
                return true;
            }
        }
    }
    if (m_header_received < Message::HeaderSize) {
        return false;
    }
    int res = ::recv(m_fd, m_msg_buffer + m_msg_received, m_msg_size - m_msg_received, MSG_DONTWAIT);
    if (res < 0) {
        throw std::runtime_error("recv(2) returned "s + std::to_string(res) + ": " +
                strerror(errno));
    }
    m_msg_received += res;
    if (m_msg_received == m_msg_size) {
        msg.fromData(m_msg_buffer, m_msg_size);
        m_msg_buffer = nullptr;
        m_header_received = 0;
        return true;
    }
    return false;
}

void TCPConnection::write(const OutMessage &msg) {
    if (msg.m_length > Message::MaxDataSize) {
        throw std::runtime_error("Cannot send a message longer than " +
            std::to_string(Message::MaxDataSize) + " bytes");
    }
    const uint16 msg_length = static_cast<uint16>(msg.m_length);
    const byte header[Message::HeaderSize] = {
        static_cast<byte>(msg_length & 0xFF),
        static_cast<byte>(msg_length >> 8),
        static_cast<byte>(msg.m_type),
        msg.m_subtype
    };

    std::memcpy(msg.m_actualData, header, Message::HeaderSize);
    int wr = ::send(m_fd, msg.m_actualData, Message::HeaderSize + msg.m_length, 0);
    if (wr == -1){
        throw std::runtime_error("send(2) returned -1: "s + strerror(errno));
    }
}

bool TCPConnection::disconnect() {
    close(m_fd);
    return true;
}

}
}
