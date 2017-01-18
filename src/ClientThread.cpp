#include "ClientThread.hpp"

#include <iostream>
#include <sstream>
#include <random>
#include <math.h>
#include <ctime>

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
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    try {
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
        cout << now->tm_mday << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec << " Disconnecting from " << tcpc.address() << ':' << tcpc.port() <<
            (client_asked_disconnect ? " (client disconnect)": "") << endl;
        tcpc.disconnect();
    } catch (const std::runtime_error &e) {
        cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Caught std::runtime_error: " << e.what() << endl;
        throw e;
    }
}

void ClientThread::start() {
    thread = std::thread(&ClientThread::run, this);
}

void ClientThread::processMessage(net::InMessage &msg, net::TCPConnection &co) {
    /*std::cout << "InMessage { type = " << static_cast<uint>(msg.getType()) <<
                 ", subtype = " << static_cast<uint>(msg.getSubtype()) <<
                 ", length = " << msg.length() << "}" << std::endl;*/
    switch (msg.getType()) {
    case net::MessageType::Auth: {
        switch(msg.getSubtype<net::AuthSubType>())
        {
            case net::AuthSubType::Request:{
                time_t t = time(0);   // get time now
                struct tm * now = localtime( & t );
                string username = msg.readString();
                string password = msg.readString();
                DatabaseConnection db("properties.txt");
                db.connect();
                auto results = db.execute("select token,password from users where username='"+username+"';");
                if(results.empty())
                {
                    db.execute("insert into users values ('"+username+"','"+password+"');");
                    results = db.execute("select token from users where username='"+username+"';");
                    net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Response);
                    omsg.writeI32(std::stoi(results[0]["token"]));
                    co.write(omsg);
                    cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Account created for user " << username << endl;
                }
                else{
                    if(password==results[0]["password"])
                    {
                        results = db.execute("select token from users where username='"+username+"';");
                        net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Response);
                        omsg.writeI32(std::stoi(results[0]["token"]));
                        co.write(omsg);
                        cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Auth granted for user " << username << endl;
                    }
                    else{
                        net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Denied);
                        co.write(omsg);
                        cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Auth denied for user " << username << endl;
                    }
                }
                db.finish();

        }break;

        default:{
                time_t t = time(0);   // get time now
                struct tm * now = localtime( & t );
                cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Auth received with no known SubType" << endl;

            }break;
        }

       // int32 token = msg.readI32();
       // cout << "AUTH! " << token << endl;
    } break;

    case net::MessageType::UserAccount:{
        switch(msg.getSubtype<net::UserAccountSubType>())
        {
            case net::UserAccountSubType::InfoRequest:{
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            net::OutMessage omsg(net::MessageType::UserAccount,(uint8)net::UserAccountSubType::InfoResponse);
            DatabaseConnection db("properties.txt");
            string str = to_string(msg.readI32());
            cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " User account requested for user with token : " << str << endl;
            db.connect();
            auto results = db.execute("select nickname,bio from accounts where token="s+str+";");
            if(results.empty())
            {
                omsg.writeString("");
                omsg.writeString("");
            }
            else
            {
                omsg.writeString(results[0]["nickname"]);
                omsg.writeString(results[0]["bio"]);
            }
            co.write(omsg);
            db.finish();
            }break;

        case net::UserAccountSubType::AccountModify:{
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            DatabaseConnection db("properties.txt");
            db.connect();
            string nick = msg.readString();
            string bio = msg.readString();
            int token = msg.readI32();
            cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " <<  now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< "Account modify requested for user with token : " << token << endl;
            auto results = db.execute("select * from accounts where token="+to_string(token)+";");
            if(results.empty())
            {
                db.execute("insert into accounts values ('"+nick+"','"+bio+"',"+to_string(token)+");");
            }
            else{
                db.execute("update accounts set (nickname,bio) = ('"+nick+"','"+bio+"') where token ="+to_string(token)+";");
            }
            db.finish();
        }break;

        }
    } break;

    case net::MessageType::Inventory: {
        time_t t = time(0);   // get time now
        struct tm * now = localtime( & t );
        cout << now->tm_mday << "-" << (now->tm_mon + 1) << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Inventory requested"<< endl;
    } break;

    case net::MessageType::PosUpdate:{
        switch(msg.getSubtype<net::PosUpdateSubType>())
        {
            case net::PosUpdateSubType::EventGet:
                {
                    time_t t = time(0);   // get time now
                    struct tm * now = localtime( & t );
                    cout <<  now->tm_mday << (now->tm_mon + 1) << '-'  << " " << now->tm_hour<<":"<<now->tm_min<<":"<<now->tm_sec<< " Position update requested for auth user" << endl;
                    double lat = msg.readDouble();
                    double lng = msg.readDouble();
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(1,10);
                    int i,j = dis(gen);
                    dis = std::uniform_int_distribution<> (-100,100);
                    net::OutMessage omsg(net::MessageType::PosUpdate,(uint8)net::PosUpdateSubType::EventGetResponse);
                    omsg.writeI32(j);
                    for(i=0;i<=j;i++)
                    {
                        double coeff1 = dis(gen)*0.0000089;;
                        double coeff2 = dis(gen);
                        double newlat=lat+coeff1;
                        double newlng=lng+coeff2/(6378137*cos(M_PI*lat/180))*180/M_PI;
                        omsg.writeDouble(newlat);
                        omsg.writeDouble(newlng);
                    }
                    co.write(omsg);
                }

        }
    }
    default:
        break;
    }
}

}
