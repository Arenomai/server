#include <functional>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <thread>
#include <unistd.h>

#include "net/DatabaseConnection.hpp"
#include "net/TCPListener.hpp"
#include "Notifiable.hpp"

using namespace std;

namespace arn {

struct ClientThreadEvent {
    enum class Type : uint8 {
      Quit
    } type;
};

class ClientThread : public Notifiable<ClientThreadEvent> {
public:
    std::thread thread;

    DatabaseConnection &conn;
    net::TCPConnection tcpc;
    int epoll_fd;

    using Event = ClientThreadEvent;

    ClientThread(DatabaseConnection &conn, net::TCPConnection &&tcpc) :
        conn(conn),
        tcpc(std::move(tcpc)) {
        epoll_fd = ::epoll_create1(0);
        epoll_event evt;
        evt.events = EPOLLIN;
        evt.data.fd = evt_fd;
        ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, evt_fd, &evt);
    }

    ~ClientThread() {
        ::close(epoll_fd);
        ::close(evt_fd);
    }

    void run() {
        cout << "Connected to " << tcpc.address() << ':' << tcpc.port() << endl;
        {
            epoll_event evt;
            evt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
            evt.data.fd = tcpc.fd();
            ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcpc.fd(), &evt);
        }
        bool cont = true;
        bool client_asked_disconnect = false;
        std::ostringstream oss;
        std::queue<std::string> lines;
        while (cont) {
            epoll_event epevt;
            int res = ::epoll_wait(epoll_fd, &epevt, 1, -1);
            if (res == -1) {
                throw std::runtime_error("epoll_wait(2) returned -1: "s + strerror(errno));
            }
            // If a fd triggered the end of the waiting period,
            if (epevt.data.fd == tcpc.fd()) {
                // it's either the TCP socket; process network data
                if ((epevt.events & (EPOLLHUP | EPOLLRDHUP)) != 0) {
                    cont = false;
                    client_asked_disconnect = true;
                } else if ((epevt.events & EPOLLIN) != 0) {
                    std::string data = tcpc.readNonblock(4096);
                    for (int i = 0; i < data.size(); ++i) {
                        const char c = data[i];
                        if (c == '\n') {
                            oss << data.substr(0, i + 1);
                            lines.emplace(oss.str());
                            oss = std::ostringstream();
                        }
                    }
                    while (not lines.empty()) {
                        const std::string &line = lines.front();
                        cout << line;
                        vector<string> results = conn.execute(line);
                        for (ssize_t i = 0; i < results.size(); ++i) {
                            tcpc.write(results[i]);
                        }
                        tcpc.write("\n");
                        lines.pop();
                    }
                }
            } else if (epevt.data.fd == evt_fd) {
                // either the notification event fd; process events
                std::lock_guard<std::mutex> evt_mtx_guard(evt_mtx);
                uint64 val;
                ::read(evt_fd, &val, sizeof(val));
                while (not evt_queue.empty()) {
                    const Event &evt = evt_queue.front();
                    switch (evt.type) {
                    case Event::Type::Quit:
                        cont = false;
                        break;
                    default:
                        break;
                    }
                    evt_queue.pop();
                }
            }
        }
        cout << "Disconnecting from " << tcpc.address() << ':' << tcpc.port() <<
            (client_asked_disconnect ? " (client disconnect)": "") << endl;
        tcpc.disconnect();
    }

    void start() {
        thread = std::thread(&ClientThread::run, this);
    }
};

bool quit = false;
Notifiable<int> main_notifier;

void handle_sigint(int sig) {
  (void) sig;
  quit = true;
  main_notifier.notify(0);
}

int main(int argc, char **argv) {
    (void) argc; (void) argv;

    DatabaseConnection conn;
    conn.connect();

    net::TCPListener tcpl;
    tcpl.listen();
    cout << "Listening on port " << tcpl.port() << endl;

    std::list<ClientThread> threads;
    int epoll_fd = ::epoll_create1(0);
    epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = main_notifier.fd();
    ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, main_notifier.fd(), &evt);
    evt.data.fd = tcpl.fd();
    ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcpl.fd(), &evt);

    struct sigaction sigint_handler;
    sigint_handler.sa_handler = handle_sigint;
    sigemptyset(&sigint_handler.sa_mask);
    sigint_handler.sa_flags = 0;
    sigaction(SIGINT, &sigint_handler, NULL);

    while (not quit) {
        int res = ::epoll_wait(epoll_fd, &evt, 1, -1);
        if (res == -1) {
            int err = errno;
            if (err != EINTR) {
                throw std::runtime_error("epoll_wait(2) returned -1: "s + strerror(err));
            }
        } else {
            if (evt.data.fd == tcpl.fd()) {
                net::TCPConnection tcpc = tcpl.accept();
                threads.emplace_back(conn, std::move(tcpc));
                threads.back().start();
            } else if (evt.data.fd == main_notifier.fd()) {
                quit = true;
            }
        }
    }
    ::close(epoll_fd);

    cout << "Exiting" << std::endl;
    // End all threads
    for (ClientThread &t : threads) {
        if (t.thread.joinable()) {
            t.notify(ClientThreadEvent { ClientThreadEvent::Type::Quit });
            t.thread.join();
        }
    }
}

}

int main(int argc, char **argv) {
    return arn::main(argc, argv);
}
