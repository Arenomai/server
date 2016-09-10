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
    int i = 0;
    file_name = "properties.txt";
    properties_file.open(file_name);

    if(properties_file.is_open())
    {
        while(getline(properties_file,line))
        {
            current_line = line.substr(0,line.find(delimiter));
            value = line.substr(line.find(delimiter)+1,line.find('\0'));
        }
    }
    else{
        cout << "Unable to open file "+file_name+" while trying to write in it";
    }

    const char *conninfo;
        PGconn     *conn;
        PGresult   *res;
        int         nFields;
        int         i,
                    j;
    infos = "hostaddr="+host+" port="+port+" dbname="+db_name+" user="+login+" password="+password;
    conninfo = infos.c_str();
    conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK)
        {
            cout << "Connection error";
        }
    else{
        cout << "Connection succeed";
    }
    return 0;
}
