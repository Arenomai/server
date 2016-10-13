#include "TCPConnection.hpp"
#include <iostream>
#include <unistd.h>
#include <sstream>
using namespace std;

TCPConnection::TCPConnection(){
    int socket_desc = -1;
}

bool TCPConnection::connect(){
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
    server.sin_port = htons(4242);
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
        {
            //print the error message
            cout << "Error : bind failed"<< endl;
            return false;
        }
    cout << "Bind success"<< endl;
    this->socket_desc = socket_desc;
    listen(this->socket_desc , 3);
    return true;
}

bool TCPConnection::accept()
{
    int c;
    struct sockaddr_in client;
    cout << "Waiting for connections" << endl;
    c = sizeof(struct sockaddr_in);
    client_sock = ::accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
        {
            cout << "Error : accept failed" << endl;
            return false;
        }
    return true;
}

string TCPConnection::read(int dataSize)
{
    std::string client_message(dataSize, '\0');
    int read_size, read_total = 0;
    while ( (read_size = recv(client_sock , &client_message[read_total] , dataSize, 0)) > 0 ) {
        read_total+=read_size;
        dataSize -= read_size;
    }
    return client_message;
}

bool TCPConnection::write(const std::string &message)
{
    int wr = send(client_sock , message.c_str(), message.length(), 0);
    if (wr == -1){
        cout << "Error while sending data" << endl;
        return false;
    }
    return true;
}

bool TCPConnection::disconnect()
{
    close(socket_desc);
    close(client_sock);
}
