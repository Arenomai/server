#include "Connection.hpp"

#include <sstream>

#include <libpq/libpq-fs.h>
#include <libpq-events.h>

#include "SimpleConfig.hpp"

using namespace std;

Connection::Connection() : Connection("properties.txt") {
}

Connection::Connection(const string &path) {
    file_name = path;
}

void Connection::connect() {
    SimpleConfig cfg;
    properties_file.open(file_name);
    if (properties_file.is_open()) { // récupération des informations de Connection
        cfg.parseText(properties_file);
        properties_file.close();
    } else {
        cout << "Unable to open file " << file_name;
    }
    std::ostringstream oss;
    static const std::array<std::string, 6> entries = {{
      "hostaddr", "host", "port", "dbname", "user", "password"
    }};
    for (const std::string &entry : entries) {
      if (not cfg.entries[entry].empty()) {
        oss << entry << '=' << cfg.entries[entry] << ' ';
      }
    }
    std::string infos = oss.str();
    const char *conninfo = infos.c_str();
    conn = PQconnectdb(conninfo); // lancement de la Connection
    if (PQstatus(conn) != CONNECTION_OK) {
        cout << "Connection error: " << PQerrorMessage(conn); // PQerrorMessage already includes \n
    } else {
        cout << "Connection succeeded" << std::endl;
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
