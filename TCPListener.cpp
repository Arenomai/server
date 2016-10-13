#include "TCPConnection.hpp"
#include <iostream>
#include <unistd.h>
#include <sstream>
using namespace std;

TCPConnection::TCPConnection() {
    socket_desc = -1;
    m_port = 4242;
    m_maxPending = 3;
}

bool TCPConnection::connect() {
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1)
    {
        cout << "Error : Socket could not be created" << endl;
    }
    cout << "Socket created" << endl;

    struct sockaddr_in client,server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(m_port);
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        // print the error message
        cout << "Error : bind failed"<< endl;
        return false;
    }
    cout << "Bind success"<< endl;
    m_fd = socket_desc;
    listen(m_fd, m_maxPending);
    return true;
}

TCPConnection TCPConnection::accept() {
    int c;
    struct sockaddr_in client;
    cout << "Waiting for connections" << endl;
    c = sizeof(struct sockaddr_in);
    int client_fd = ::accept(m_fd, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_fd < 0) {
        cout << "Error : accept failed" << endl;
        return false;
    }
    return true;
}

bool TCPConnection::disconnect() {
    close(m_fd);
}
