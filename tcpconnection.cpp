#include "tcpconnection.hpp"
#include <iostream>

using namespace std;

TCPconnection::TCPconnection(){
    int socket_desc = -1;
}

bool TCPconnection::connect(std::string address){
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1)
    {
        cout << "Error : Socket could not be created";
    }
    cout << "Socket created\n";

    struct sockaddr_in client,server;
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(4242);
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
        {
            //print the error message
            cout << "Error : bind failed";
            return false;
        }
    cout << "Bind success\n";
    this->socket_desc = socket_desc;
    return true;
}

bool TCPconnection::wait()
{
    int c;
    struct sockaddr_in client;
    listen(socket_desc , 3);
    cout << "Waiting for connections";
    c = sizeof(struct sockaddr_in);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
        {
            cout << "Error : accept failed";
            return false;
        }
    return true;
}

std::vector<char> TCPconnection::read(int dataSize)
{
    std::vector<char> client_message(dataSize);
    int read_size, read_total=0;
    while( (read_size = recv(client_sock , client_message.data()+read_total , dataSize, 0)) > 0 )
        {
        read_total+=read_size;
        dataSize -= read_size;
    }
    for (std::vector<char>::const_iterator i = client_message.begin(); i != client_message.end(); ++i)
        std::cout << *i << ' ';
    return client_message;
}
