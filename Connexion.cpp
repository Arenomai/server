#include "Connexion.hpp"
#include <libpq/libpq-fs.h>
#include <libpq-events.h>
#include <libpq-fe.h>
#include <iostream>
#include <fstream>
using namespace std;


    Connexion::Connexion()
        {
        file_name = "properties.txt";
        delimiter = "=";
    }

    Connexion::Connexion(string path)
    {
        file_name = path;
        delimiter = "=";
    }

    Connexion::Connexion(string path,string separation){
        file_name = path;
        delimiter = separation;
    }

    void Connexion::connect(){
        string line, current_line, value, host, port, db_name, login, password, infos;
        const char* conninfo;
        properties_file.open(file_name);
        if(properties_file.is_open()) // récupération des informations de connexion
        {
           while(getline(properties_file,line))
           {
             current_line = line.substr(0,line.find(delimiter));
             value = line.substr(line.find(delimiter)+1,line.find('\0'));
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
         if (PQstatus(conn) != CONNECTION_OK)
             {
                 cout << "Connection error\n";
             }
         else{
                 cout << "Connection succeed\n";
             }
    }

    void Connexion::execute(std::vector<string> &results,string command)
    {
        PGresult   *res;
        res = PQexec(conn, command.c_str()); // execution de la requête SQL
        int line_number = PQnfields(res), i;
        for(i=0;i<line_number;i++)
        {
            results.push_back(PQfname(res, i));
        }
    }
    void Connexion::exit()
        {
        PQfinish(conn);
        cout << "Connection closed";
    }
