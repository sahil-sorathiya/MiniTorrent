#include "../headers.h"

//: Declaration
// class User {
// public:

//     User(string userId, string password);

//     void printValue();
//     string userId;
//     string password;
//     bool isLoggedIn;
//     string ip;
//     int port;
    
// };

User::User(string userId, string password) {
    this->userId = userId;
    this->password = password;
    this->isLoggedIn = false;
    this->ip = "";
    this->port = -1;
    this->groupId = "";
}


void User::printValue(){
    cout << "userId : " << setw(15) << userId << " || password : " << setw(15)  << password  << " || isLoggedIn : " << setw(6) << boolalpha << isLoggedIn << " || ip : " << setw(16)  << ip << " || port : " << setw(5)  << port << " || groupId" << setw(15) << groupId << endl;
    return;
}