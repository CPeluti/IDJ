 	

// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
    string line;
    ifstream myfile;
    myfile.open("./resources/map/map.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,line, ',') )
        {
            cout << stoi(line) << '\n';
        }
        myfile.close();
    }

    else cout << "Unable to open file"; 

    return 0;
}