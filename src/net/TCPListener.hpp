#ifndef ARN_NET_TCPLISTENER_HPP
#define ARN_NET_TCPLISTENER_HPP

#include "TCPConnection.hpp"
#include "Types.hpp"

namespace arn {
namespace net {

class TCPListener {
private:
    int m_fd;
    Port m_port;
    int m_max_pending;

public:
    TCPListener();
    TCPListener(const TCPListener&) = delete;
    ~TCPListener();

    int fd() const {
        return m_fd;
    }

    Port port() const {
      return m_port;
    }

    bool listen();
    TCPConnection accept();
    bool disconnect();

};

}
}

#endif /* ARN_NET_TCPLISTENER_HPP */
