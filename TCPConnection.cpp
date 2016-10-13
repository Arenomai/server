#include "TCPConnection.hpp"
#include <iostream>
#include <unistd.h>
#include <sstream>
using namespace std;

TCPConnection::TCPConnection(int fd) :
    m_fd(fd) {
}

string TCPConnection::read(int dataSize) {
    std::string client_message(dataSize, '\0');
    int read_size, read_total = 0;
    while ( (read_size = recv(m_fd, &client_message[read_total] , dataSize, 0)) > 0 ) {
        read_total+=read_size;
        dataSize -= read_size;
    }
    return client_message;
}

bool TCPConnection::write(const std::string &message) {
    int wr = send(m_fd, message.c_str(), message.length(), 0);
    if (wr == -1){
        cout << "Error while sending data" << endl;
        return false;
    }
    return true;
}

bool TCPConnection::disconnect() {
    close(m_fd);
}
