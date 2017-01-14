#include "DatabaseConnection.hpp"

#include <sstream>
#include <map>

#include <libpq/libpq-fs.h>
#include <libpq-fe.h>
#include <libpq-events.h>

#include "../SimpleConfig.hpp"

using namespace std;

DatabaseConnection::DatabaseConnection() : DatabaseConnection("properties.txt") {
}

DatabaseConnection::DatabaseConnection(const string &path) {
    file_name = path;
}

DatabaseConnection::~DatabaseConnection() {
    finish();
}

void DatabaseConnection::connect() {
    SimpleConfig cfg;
    properties_file.open(file_name);
    if (properties_file.is_open()) { // récupération des informations de Connection
        cfg.parseText(properties_file);
        properties_file.close();
    } else {
        throw std::runtime_error("Unable to open file " + file_name);
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
        throw std::runtime_error("PGconn error: "s + PQerrorMessage(conn));
    }
}

std::vector<std::map<std::string, std::string>> DatabaseConnection::execute(const string &command) {
    std::vector<std::map<std::string, std::string>> results;

    PGresult *res = PQexec(conn, command.c_str()); // execution de la requête SQL

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        throw std::runtime_error(PQresultErrorMessage(res));
    }

    int column_count = PQnfields(res);
    std::vector<const char*> column_names(column_count);
    for (int c = 0; c < column_count; ++c) {
      column_names[c] = PQfname(res, c);
    }

    const int line_count = PQntuples(res);
    for (int l = 0; l < line_count; ++l) {
        results.emplace_back();
        std::map<std::string, std::string> &line = results.back();
        for (int c = 0; c < column_count; ++c) {
            line.emplace(std::piecewise_construct,
                std::forward_as_tuple(column_names[c]),
                std::forward_as_tuple(PQgetvalue(res, l, c)));
        }
    }
    PQclear(res);

    return results;
}

void DatabaseConnection::finish() {
    if (conn != nullptr) {
        PQfinish(conn);
        conn = nullptr;
    }
}
