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
#include "ClientThread.hpp"
#include "Notifiable.hpp"

using namespace std;

namespace arn {

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
