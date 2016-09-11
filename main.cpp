#include <fstream>
#include <iostream>
#include <vector>

#include "Connection.hpp"

using namespace std;

int main(int argc, char **argv) {
    (void) argc; (void) argv;

    Connection conn;
    conn.connect();
    vector<string> results = conn.execute("select * from test");
    for (unsigned int i = 0; i < results.size(); ++i) {
        cout << results[i] << endl;
    }
    conn.exit();

    return 0;
}
