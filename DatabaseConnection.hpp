#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <fstream>
#include <iostream>
#include <vector>

#include <libpq-fe.h>

class DatabaseConnection {
public:
        // Constructeurs
    DatabaseConnection();
    DatabaseConnection(const std::string &path);

    void connect();
    std::vector<std::string> execute(const std::string &command);
    void exit();

private:
    std::ifstream properties_file;
    std::string file_name;
    PGconn *conn;
};

#endif // CONNECTION_HPP
