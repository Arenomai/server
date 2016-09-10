#include <iostream>
#include <fstream>
#include "Connexion.hpp"

using namespace std;

int main()
{
    int i;
    Connexion conn = Connexion();
    conn.connect();
    vector<string> results;
    conn.execute(results, "select * from test");
    for(i=0;i<results.size();i++)
    {
    cout << results.at(i)+"\n";
    }
    conn.exit();
    return 0;
}
