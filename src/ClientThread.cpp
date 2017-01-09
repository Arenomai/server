#include "ClientThread.hpp"

#include <iostream>
#include <sstream>

#include "net/Network.hpp"

using namespace std;

namespace arn {

ClientThread::ClientThread(DatabaseConnection &conn, net::TCPConnection &&tcpc) :
    conn(conn),
    tcpc(std::move(tcpc)) {
    epoll_fd = ::epoll_create1(0);
    epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = evt_fd;
    ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, evt_fd, &evt);
}

ClientThread::~ClientThread() {
    ::close(epoll_fd);
    ::close(evt_fd);
}

void ClientThread::run() {
    cout << "Connected to " << tcpc.address() << ':' << tcpc.port() << endl;
    {
        epoll_event evt;
        evt.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
        evt.data.fd = tcpc.fd();
        ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcpc.fd(), &evt);
    }
    bool cont = true;
    bool client_asked_disconnect = false;
    net::InMessage msg;
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
                while (tcpc.read(msg)) {
                    processMessage(msg, tcpc);
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

void ClientThread::start() {
    thread = std::thread(&ClientThread::run, this);
}

void ClientThread::processMessage(net::InMessage &msg, net::TCPConnection &co) {
    std::cout << "InMessage { type = " << static_cast<uint>(msg.getType()) <<
                 ", subtype = " << static_cast<uint>(msg.getSubtype()) <<
                 ", length = " << msg.length() << "}" << std::endl;
    switch (msg.getType()) {
    case net::MessageType::Auth: {
        switch(msg.getSubtype<net::AuthSubType>())
        {
            case net::AuthSubType::Request:{
                string username = msg.readString();
                string password = msg.readString();
                DatabaseConnection db("properties.txt");
                db.connect();
                auto results = db.execute("select token,password from users where username='"+username+"';");
                if(results.empty())
                {
                    db.execute("insert into users values ('"+username+"','"+password+"');");
                    cout << "db" << endl;
                    results = db.execute("select token from users where username='"+username+"';");
                    net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Response);
                    omsg.writeI32(std::stoi(results[0]["token"]));
                    co.write(omsg);
                    cout << "Account created for user " << username << endl;
                }
                else{
                    if(password==results[0]["password"])
                    {
                        results = db.execute("select token from users where username='"+username+"';");
                        net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Response);
                        omsg.writeI32(std::stoi(results[0]["token"]));
                        co.write(omsg);
                        cout << "Auth granted for user " << username << endl;
                    }
                    else{
                        net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Denied);
                        co.write(omsg);
                        cout << "Auth denied for user " << username << endl;
                    }
                }

        }break;

            default:
                cout << "Auth received with no known SubType" << endl;
            break;
        }

       // int32 token = msg.readI32();
       // cout << "AUTH! " << token << endl;
    } break;

    case net::MessageType::Inventory: {
        cout << "Inventory requested?"<< endl;
    } break;

    default:
        break;
    }
}

}
