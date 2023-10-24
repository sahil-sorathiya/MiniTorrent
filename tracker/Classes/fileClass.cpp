#include "../headers.h"

//: Declaration
// class File {
// public:
    
//     File(string fileName, string SHA, int bytes);

//     void printValue();
//     string fileName;
//     string SHA;
//     int bytes;
//     vector <string> ipAndPorts;

// };

File::File(string fileName, string SHA, int bytes){
    this->fileName = fileName;
    this->SHA = SHA;
    this->bytes = bytes;
}

void File::printValue(){
    cout << "fileName : " << setw(20) << fileName << "," << endl 
        << "SHA : " << setw(21)  << SHA << endl
        << "bytes : " << setw(12) << bytes << endl
        << "ipAndPorts : ";
    
    for(auto it: ipAndPorts){
        cout << it << " ";
    }
    cout << endl;
    return;
}