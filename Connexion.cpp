#include "Connexion.hpp"
#include <libpq/libpq-fs.h>
#include <libpq-events.h>
#include <libpq-fe.h>
#include <iostream>

using namespace std;

Connexion::Connexion()
{
    Connexion::Connexion()
    {
        this->file_name = "properties.txt";
        this->delimiter = "=";
    }

    Connexion::Connexion(path)
    {
        this->file_name = path;
        this->delimiter = "=";
    }

    Connexion::Connexion(path,separation){
        this->file_name = path;
        this->delimiter = separation;
    }

    void Connexion::connect(){
        string line, current_line, value, host, port, db_name, login, password, infos;
        const char* conninfo;
        this->properties_file.open(this->file_name);
        if(this->properties_file.is_open()) // récupération des informations de connexion
        {
           while(getline(this->properties_file,line))
           {
             current_line = line.substr(0,line.find(this->delimiter));
             value = line.substr(line.find(this->delimiter)+1,line.find('\0'));
             if(current_line =="host"){
                 host = value;
             }
             if(current_line == "port"){
                 port = value;
             }
             if(current_line == "db_name"){
                 db_name = value;
             }
             if(current_line == "login"){
                 login = value;
             }
             if(current_line == "password"){
                 password = value;
             }
           }
         }
         else{
             cout << "Unable to open file "+file_name+" while trying to write in it";
         }
         infos = "hostaddr="+host+" port="+port+" dbname="+db_name+" user="+login+" password="+password;
         conninfo = infos.c_str();
         conn = PQconnectdb(conninfo); // lancement de la connexion
         if (PQstatus(this->conn) != CONNECTION_OK)
             {
                 cout << "Connection error\n";
             }
         else{
                 cout << "Connection succeed\n";
             }
    }

    string[] Connexion::execute(command)
    {
        PGresult   *res;

        res = PQexec(this->conn, command); // execution de la requête SQL
        line_number = PQnfields(res);
        string results[line_number];
        for(i=0;i<line_number;i++)
        {
            result[i] =PQfname(res, i);
        }
        return result;
    }
    void Connexion::exit()
        {
        PQfinish(this->conn);
    }
}
