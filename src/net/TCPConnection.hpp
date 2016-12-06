#ifndef ARN_NET_TCPCONNECTION_HPP
#define ARN_NET_TCPCONNECTION_HPP

#include <arpa/inet.h>
#include <string>

#include "Network.hpp"
#include "Types.hpp"

namespace arn {
namespace net {

class TCPConnection {
private:
    int m_fd;
    struct sockaddr_in m_addr;

    Message::Header m_header_buffer;
    uint m_header_received;

    byte *m_msg_buffer;
    uint m_msg_received, m_msg_size;

public:
    TCPConnection(int fd, struct sockaddr_in addr);
    TCPConnection(const TCPConnection&) = delete;
    TCPConnection(TCPConnection&&);

    TCPConnection& operator=(const TCPConnection&) = delete;
    TCPConnection& operator=(TCPConnection&&);

    int fd() const {
        return m_fd;
    }

    bool connected() const {
        return m_fd >= 0;
    }

    Port port() const;
    std::string address() const;

    [[deprecated]] std::string read(int dataSize);
    [[deprecated]] std::string readNonblock(int max_size);
    [[deprecated]] void write(const std::string &message);
    bool read(InMessage&);
    void write(const OutMessage&);
    bool disconnect();

};

}
}

#endif /* ARN_NET_TCPCONNECTION_HPP */
