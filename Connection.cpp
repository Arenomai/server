#include "Connection.hpp"

#include <sstream>
#include <libpq/libpq-fs.h>
#include <libpq-events.h>

using namespace std;

Connection::Connection() {
    file_name = "properties.txt";
    delimiter = "=";
}

Connection::Connection(const string &path) {
    file_name = path;
    delimiter = "=";
}

Connection::Connection(const string &path, const string &separation){
    file_name = path;
    delimiter = separation;
}

void Connection::connect() {
    string line, current_line, value, host, port, db_name, login, password, infos;
    std::ostringstream oss;
    const char* conninfo;
    properties_file.open(file_name);
    if (properties_file.is_open()) { // récupération des informations de Connection
        while (getline(properties_file,line)) {
            current_line = line.substr(0,line.find(delimiter));
            value = line.substr(line.find(delimiter)+1,line.find('\0'));
            if (current_line =="host") {
                host = value;
            }
            if (current_line == "port") {
                port = value;
            }
            if (current_line == "db_name") {
                db_name = value;
            }
            if (current_line == "login") {
                login = value;
            }
            if (current_line == "password") {
                password = value;
            }
        }
    } else {
        cout << "Unable to open file "+file_name+" while trying to write in it";
    }
    oss << "hostaddr=" << host <<" port="<<port<<" dbname="<<db_name<<" user="<<login<<" password="<<password;
    infos = oss.str();
    conninfo = infos.c_str();
    conn = PQconnectdb(conninfo); // lancement de la Connection
    if (PQstatus(conn) != CONNECTION_OK) {
        cout << "Connection error\n";
    } else {
        cout << "Connection succeed\n";
    }
}

std::vector<std::string> Connection::execute(const string &command) {
    std::vector<std::string> results;
    PGresult *res = PQexec(conn, command.c_str()); // execution de la requête SQL
    int column_count = PQnfields(res);
    std::vector<const char*> column_names(column_count);
    for (int c = 0; c < column_count; ++c) {
      column_names[c] = PQfname(res, c);
    }
    int line_count = PQntuples(res);
    std::ostringstream oss;
    for (int l = 0; l < line_count; ++l) {
        oss.str("");
        oss.clear();
        for (int c = 0; c < column_count; ++c) {
          oss << column_names[c] << '=' << PQgetvalue(res, l, c) << ' ';
        }
        results.push_back(oss.str());
    }
    PQclear(res);
    return results;
}

void Connection::exit() {
    PQfinish(conn);
    cout << "Connection closed";
}
