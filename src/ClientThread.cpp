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
    try {

        stringstream ss;
        ss <<" Connected to " << tcpc.address() << ':' << tcpc.port();
        printInfo(ss.str());

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

        ss=stringstream();
        ss << " Disconnecting from " << tcpc.address() << ':' << tcpc.port() << (client_asked_disconnect ? " (client disconnect)": "");
        printInfo(ss.str());
        tcpc.disconnect();
    } catch (const std::runtime_error &e) {
        stringstream ss;
        ss << " Caught std::runtime_error: " << e.what() << endl;
        printInfo(ss.str());
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
    stringstream ss;
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
                    results = db.execute("select token from users where username='"+username+"';");
                    net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Response);
                    omsg.writeI32(std::stoi(results[0]["token"]));
                    co.write(omsg);
                    ss=stringstream();
                    ss <<" Account created for user " << username;
                    printInfo(ss.str());
                }
                else{
                    if(password==results[0]["password"])
                    {
                        results = db.execute("select token from users where username='"+username+"';");
                        net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Response);
                        omsg.writeI32(std::stoi(results[0]["token"]));
                        co.write(omsg);
                        ss=stringstream();
                        ss << " Auth granted for user " << username;
                        printInfo(ss.str());
                    }
                    else{ //If the password is not matching an existing username
                        net::OutMessage omsg(net::MessageType::Auth,(uint8)net::AuthSubType::Denied);
                        co.write(omsg);
                        ss=stringstream();
                        ss << " Auth denied for user " << username;
                        printInfo(ss.str());
                    }
                }
                db.finish();

        }break;

        default:{
                printInfo(" Auth received with no known SubType");

            }break;
        }

       // int32 token = msg.readI32();
       // cout << "AUTH! " << token << endl;
    } break;

    case net::MessageType::UserAccount:{
        switch(msg.getSubtype<net::UserAccountSubType>())
        {
            case net::UserAccountSubType::InfoRequest:{

            net::OutMessage omsg(net::MessageType::UserAccount,(uint8)net::UserAccountSubType::InfoResponse);

                DatabaseConnection db("properties.txt");
                string str = to_string(msg.readI32());

                ss=stringstream();
                ss <<" User account requested for user with token : " << str;
                printInfo(ss.str());

                db.connect();
                //Works even if displayed as a synthax error in QtCreator
                auto results = db.execute("select nickname,bio from accounts where token="s+str+";");

                if(results.empty()) // There is no account info with the token given by the auth method
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
                DatabaseConnection db("properties.txt");
                db.connect();
                string nick = msg.readString();
                string bio = msg.readString();
                int token = msg.readI32();
                ss=stringstream();
                ss <<" Account modify requested for user with token : " << token;
                printInfo(ss.str());
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
        ss=stringstream();
        ss <<" Inventory requested";
        printInfo(ss.str());

        //TODO : Implement the inventory system

    } break;

    case net::MessageType::PosUpdate:{
        switch(msg.getSubtype<net::PosUpdateSubType>())
        {
            case net::PosUpdateSubType::EventGet:
                {

                /* This function works, however we have problems with GPS sync, so we should
                 * add a sleep time before creating the events. I don't think doing that
                 * with the server is a good idea, maybe wait for a stable and accurate position of the client before
                 * requesting processing.*/

                    ss=stringstream();
                    ss <<" Position update requested for authenticated user";
                    printInfo(ss.str());
                    double lat = msg.readDouble();
                    double lng = msg.readDouble();

                    //Part generating almost real random numbers
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(1,10);
                    int i,j = dis(gen);
                    dis = std::uniform_int_distribution<> (-100,100);

                    net::OutMessage omsg(net::MessageType::PosUpdate,(uint8)net::PosUpdateSubType::EventGetResponse);
                    omsg.writeI32(j);
                    for(i=0;i<=j;i++)
                    {
                        double coeff1 = dis(gen)*0.0000089;
                        double coeff2 = dis(gen);

                        //These are some hacks to compute some latitute and longitude accurately without using too complex formulas
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


void ClientThread::printInfo(std::string msg)
{
    /* This function is meant to create a string with a correct and coherent timestamp before displaying a message
     * on the console.
     *
     * TODO : A log file ? Because this is not a system daemon or whatever, we shouldn't log the messages in syslog.
     */

stringstream ss;
time_t t = time(0);   //Get current time

struct tm * now = localtime( & t );

if(now->tm_mday < 10)
{
    ss << "0" << now->tm_mday <<"-";
}
else{
    ss << now->tm_mday << "-";
}
if((now->tm_mon+1)<10)
{
    ss << "0" << (now->tm_mon+1) << " ";
}
else{
    ss << (now->tm_mon+1) << " ";
}
if(now->tm_hour < 10)
{
    ss << "0" << now->tm_hour<<":";
}
else{
    ss << now->tm_hour<<":";
}
if(now->tm_min <10)
{
    ss << "0" << now->tm_min <<":";
}
else
{
    ss << now->tm_min<<":";
}
if(now->tm_sec <10)
{
    ss << "0" << now->tm_sec <<" ";
}
else{
    ss << now->tm_sec << " ";
}

ss << msg;
 cout << ss.str() <<endl;

}
}
