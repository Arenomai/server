#ifndef Connection_HPP
#define Connection_HPP

#include <fstream>
#include <iostream>
#include <vector>

#include <libpq-fe.h>

class Connection {
public:
        // Constructeurs
    Connection();
    Connection(const std::string &path);
    Connection(const std::string &path, const std::string &separation);

    void connect();
    std::vector<std::string> execute(const std::string &command);
    void exit();

private:
    std::ifstream properties_file;
    std::string file_name, delimiter;
    PGconn *conn;
};

#endif // Connection_HPP
