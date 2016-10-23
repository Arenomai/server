#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include "DatabaseConnection.hpp"
#include "TCPListener.hpp"

using namespace std;

int main(int argc, char **argv) {
    (void) argc; (void) argv;

    DatabaseConnection conn;
    conn.connect();

    string result;
    TCPListener tcpl;
    bool quit = false;
    tcpl.listen();
    while (not quit) {
        TCPConnection tcpc = tcpl.accept();
        cout << "Connected to " << tcpc.address() << ':' << tcpc.port() << endl;
        std::ostringstream oss;
        while (result != ";") {
            result = tcpc.read(1);
            if (result == "µ") {
                quit = true;
            } else {
                cout << result;
                oss << result;
            }
        }
        cout << endl;
        oss << endl;
        if (not quit) {
          vector<string> results = conn.execute(oss.str());
          for (unsigned int i = 0; i < results.size(); ++i) {
              tcpc.write(results[i]);
          }
          cout << "Disconnecting from " << tcpc.address() << ':' << tcpc.port() << endl;
          tcpc.disconnect();
        }
    }
}
