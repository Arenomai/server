#include <fstream>
#include <iostream>
#include <vector>

#include "DatabaseConnection.hpp"
#include "TCPListener.hpp"

using namespace std;

int main(int argc, char **argv) {
    (void) argc; (void) argv;

    /*DatabaseConnection conn;
    conn.connect();
    vector<string> results = conn.execute("select * from test");
    for (unsigned int i = 0; i < results.size(); ++i) {
        cout << results[i] << endl;
    }
    conn.exit();*/

    string result;
    TCPListener tcpl;
    tcpl.listen();
    TCPConnection tcpc = tcpl.accept();
    while (result != "exit") {
        result = tcpc.read(4);
        tcpc.write(result);
        cout << result;// << endl;
        cout.flush();
    }

    //    cout << "Error : connection could not be created" << endl;


    return 0;
}
