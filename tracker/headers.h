#include <bits/stdc++.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <arpa/inet.h>
#include <algorithm>
// #include <openssl/sha.h>

using namespace std;

class User {
public:

    User(string userId, string password);

    void printValue();
    string userId;
    string password;
    bool isLoggedIn;
    string ip;
    int port;
    string groupId;
    unordered_set <string> files;
    
};

class File {
public:
    
    File(string fileName, string SHA, int bytes);

    void printValue();
    string fileName;
    string SHA;
    int bytes;
    unordered_set <string> ipAndPorts;

};

class Group {
public:

    Group(string groupId, string adminId);

    void printValue();
    string adminId;
    string groupId;
    unordered_set <string> pendingRequests;
    unordered_set <string> participants;  
    unordered_map <string, File> filesOfGroup;
};



extern string logFileName;
extern unordered_map <string, User> registeredUsers;
extern unordered_map <int, string> socketToUser;
extern unordered_map <string, Group> groups;

//: utils.cpp
void clearLog();
void logger(string txt);
pair <string, int> validateArgs(int argc, char *argv[]);
void printRegisteredUserMap();
void printSocketToUserMap();
void printGroupsMap();
string trimString(string str);

//: handlers.cpp
void handleClientRequest(int client_socket, string clientIp, int clientPort);
vector <string> tokenization(string input, char delimeter);
string executeCommand(vector<string> tokens, int client_socket, string clientIp, int clientPort);
string createUser(string userId, string password);
string login(string userId, string password, int client_socket, string clientIp, int clientPort);
string logout(int client_socket);
string createGroup(int client_socket, string groupId);
string joinGroup(int client_socket, string groupId);
string leaveGroup(int client_socket, string groupId);
string listRequests(int client_socket, string groupId);
string acceptRequest(int client_socket, string groupId, string userIdOfPending);
string listGroups(int client_socket);
void removeFilesOfUserFromGroup(string userId);
string uploadFilesToGroup(int client_socket, string groupId, string fileName, string SHA, int bytes);
string downloadFile(int client_socket, string groupId, string fileName);
string listFiles(int client_socket, string groupId);
void handleQuitOfTracker(int tracker_socket);
