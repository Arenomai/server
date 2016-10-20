#include "TCPConnection.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

TCPConnection::TCPConnection(int fd) :
    m_fd(fd) {
}

TCPConnection::TCPConnection(TCPConnection &&c) {
    std::swap(m_fd, c.m_fd);
}

TCPConnection& TCPConnection::operator=(TCPConnection &&c) {
    std::swap(m_fd, c.m_fd);
    return *this;
}

string TCPConnection::read(int dataSize) {
    std::string client_message(dataSize, '\0');
    int read_size, read_total = 0;
    while ( (read_size = ::recv(m_fd, &client_message[read_total] , dataSize, 0)) > 0 ) {
        read_total+=read_size;
        dataSize -= read_size;
    }
    return client_message;
}

bool TCPConnection::write(const std::string &message) {
    int wr = ::send(m_fd, message.c_str(), message.length(), 0);
    if (wr == -1){
        cout << "Error while sending data" << endl;
        return false;
    }
    return true;
}

bool TCPConnection::disconnect() {
    close(m_fd);
    return true;
}
