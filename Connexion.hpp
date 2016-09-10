#ifndef CONNEXION_HPP
#define CONNEXION_HPP
#include <libpq/libpq-fs.h>
#include <libpq-events.h>
#include <libpq-fe.h>
#include <iostream>
#include <fstream>
#include <vector>
class Connexion
{
public:
        // Constructeurs
    Connexion();
    Connexion(std::string path);
    Connexion(std::string path, std::string separation);


    void connect();
    void execute(std::vector<std::string> &results, std::string command);
    void exit();

private:
    std::ifstream properties_file;
    std::string file_name, delimiter;
    PGconn *conn;
};

#endif // CONNEXION_HPP
