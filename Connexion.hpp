#ifndef CONNEXION_HPP
#define CONNEXION_HPP
#include <libpq/libpq-fs.h>
#include <libpq-events.h>
#include <libpq-fe.h>
#include <iostream>


class Connexion
{
public:
        // Constructeurs
    Connexion();
    Connexion(std::string path);


    int connect();
    int execute(std::string command);
    int exit();

private:
    std::ifstream properties_file;
    std::string file_name, delimiter = "=";
    PGconn *conn;
};

#endif // CONNEXION_HPP
