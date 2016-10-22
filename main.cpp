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

    string result;
    TCPListener tcpl;
    bool quit;
    tcpl.listen();
    while(quit != true)
    {

        TCPConnection tcpc = tcpl.accept();
        std::ostringstream oss;
        while (result != ";") {
            result = tcpc.read(1);
            if(result == "µ")
            {
                quit = true;
            }
            else{
            cout << result;
            oss << result;
            }

        }
        cout << endl;
        oss << endl;
        if(quit!=true)
        {
        DatabaseConnection conn;
        conn.connect();
        vector<string> results = conn.execute(oss.str());
        for (unsigned int i = 0; i < results.size(); ++i) {
            tcpc.write(results[i]);
        }
        conn.exit();
        tcpc.disconnect();
        }
    }
}
