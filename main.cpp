#include <fstream>
#include <iostream>
#include <vector>

#include "DatabaseConnection.hpp"
#include "TCPConnection.hpp"
using namespace std;

int main(int argc, char **argv) {
    (void) argc; (void) argv;

     string result;
    /*DatabaseConnection conn;
    conn.connect();
    vector<string> results = conn.execute("select * from test");
    for (unsigned int i = 0; i < results.size(); ++i) {
        cout << results[i] << endl;
    }
    conn.exit();*/
    TCPconnection TCPconn;
    TCPconn.connect();
    TCPconn.accept();

    while (result != "j") {
        result = TCPconn.read(1);
        cout << result; // << endl;
        cout.flush();
    }
    TCPconn.disconnect();


    return 0;
}
