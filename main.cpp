#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    fstream test;
    string line;
    string filename;
    filename = "test.txt";
    test.open(filename, ios::in);
    if(test.is_open())
    {
    test << "that's a test bro";
    test.close();
    }
    else{
        cout << "Unable to open file "+filename+" while trying to write in it";
    }
    test.open(filename, ios::out);
    if(test.is_open())
    {
        while(getline(test,line))
        {
            cout << line;
        }
    }
    else{
        cout << "Unable to open file "+filename+" while trying to write in it";
    }
    return 0;
}
