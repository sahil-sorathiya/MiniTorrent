#include "headers.h"

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

File::File(string filePath, string fileName, string SHA, int bytes){
    this->filePath = filePath;
    this->fileName = fileName;
    this->SHA = SHA;
    this->bytes = bytes;
    this->isChunkAvailable = {};
}

void File::printValue(){
    cout << "fileName : "  << fileName << "," << endl 
        << "SHA : "   << SHA << endl
        << "bytes : "  << bytes << endl
        << "path : "  << filePath << endl
        << "isChunkAvailable : ";
    
    int c = 0;
    for(auto it: isChunkAvailable){
        cout << c++ << ":" << boolalpha << it << " ";
    }
    cout << endl;
    return;
}