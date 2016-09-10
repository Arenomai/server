#include <iostream>
#include <fstream>
#include <libpq/libpq-fs.h>
#include <libpq-events.h>
#include <libpq-fe.h>

using namespace std;

int main(int argc, char *argv[])
{
  ifstream properties_file;
  string line, current_line, file_name, delimiter = "=", value, host, port, db_name, login, password, infos;
  int line_number, i;
  file_name = "properties.txt";
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

    const char *conninfo; // création des informations nécessaires à la db
        PGconn     *conn;
        PGresult   *res;
    infos = "hostaddr="+host+" port="+port+" dbname="+db_name+" user="+login+" password="+password;
    conninfo = infos.c_str();
    conn = PQconnectdb(conninfo); // lancement de la connexion
    if (PQstatus(conn) != CONNECTION_OK)
        {
            cout << "Connection error";
        }
    else{
        cout << "Connection succeed\n";
        cout << "Content of test table :\n";
        res = PQexec(conn, "SELECT * FROM test"); // execution de la requête SQL
        line_number = PQnfields(res);
        for(i=0;i<line_number;i++)
        {
            printf("%-15s", PQfname(res, i));
            cout << "\n\n";
        }

    }
    return 0;
}
