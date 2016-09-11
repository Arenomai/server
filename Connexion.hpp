#ifndef CONNEXION_HPP
#define CONNEXION_HPP

#include <fstream>
#include <iostream>
#include <vector>

#include <libpq-fe.h>

class Connexion {
public:
        // Constructeurs
    Connexion();
    Connexion(const std::string &path);
    Connexion(const std::string &path, const std::string &separation);

    void connect();
    std::vector<std::string> execute(const std::string &command);
    void exit();

private:
    std::ifstream properties_file;
    std::string file_name, delimiter;
    PGconn *conn;
};

#endif // CONNEXION_HPP
