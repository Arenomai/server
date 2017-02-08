#ifndef ARN_CLIENT_THREAD_HPP
#define ARN_CLIENT_THREAD_HPP

#include <sys/epoll.h>
#include <thread>

#include "net/DatabaseConnection.hpp"
#include "net/TCPConnection.hpp"
#include "platform/Types.hpp"
#include "Notifiable.hpp"

namespace arn {

namespace net {
class InMessage;
}

struct ClientThreadEvent {
    enum class Type : uint8 {
      Quit
    } type;
};

class ClientThread : public Notifiable<ClientThreadEvent> {
private:
    void processMessage(net::InMessage&,net::TCPConnection&);
    void printInfo(std::string msg);

public:
    std::thread thread;

    DatabaseConnection &conn;
    net::TCPConnection tcpc;
    int epoll_fd;

    using Event = ClientThreadEvent;

    ClientThread(DatabaseConnection &conn, net::TCPConnection &&tcpc);
    ~ClientThread();

    void run();
    void start();
};

}

#endif /* ARN_CLIENT_THREAD_HPP */
