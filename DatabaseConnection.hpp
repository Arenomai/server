#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <fstream>
#include <iostream>
#include <vector>

typedef struct pg_conn PGconn;

class DatabaseConnection {
public:
    // Constructors
    DatabaseConnection();
    DatabaseConnection(const std::string &path);
    // Destructor
    ~DatabaseConnection();

    void connect();
    std::vector<std::string> execute(const std::string &command);
    void finish();

private:
    std::ifstream properties_file;
    std::string file_name;
    PGconn *conn;
};

#endif // CONNECTION_HPP
