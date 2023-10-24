#include "headers.h"

// ./tracker txts/tracker_info.txt 1
// ./client 127.0.0.1:5500 txts/tracker_info.txt

string logFileName;
unordered_map <string, File> files;
vector <unordered_set <string> > chunkStatusOfOthers;
vector <pair<int,int> > countOfClientForEachChunk;

int main(int argc, char *argv[]){

    //: Validation of args
    pair< pair<string, int>, pair <string, int> > ipAndPortInfo = validateArgs(argc, argv);

    pair<string, int> clientIpAndPort = ipAndPortInfo.first;
    pair<string, int> trackerIpAndPort = ipAndPortInfo.second;

    int seederSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (seederSocket == -1) {
        cerr << "Error creating seederSocket" << endl;
        return 1;
    }

    logger("SeederSocket Created");
    struct sockaddr_in seederAddr;
    seederAddr.sin_family = AF_INET;
    seederAddr.sin_port = htons(clientIpAndPort.second);  // Replace with your desired port number
    seederAddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(seederSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cerr << "setsockopt";
        exit(EXIT_FAILURE);
    }

    if (inet_pton(AF_INET, clientIpAndPort.first.c_str(), &seederAddr.sin_addr) <= 0){
        cerr << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    // Bind the socket to the specified address and port
    if (bind(seederSocket, (struct sockaddr*)&seederAddr, sizeof(seederAddr)) == -1) {
        cerr << "Error binding socket" << endl;
        close(seederSocket);
        return 1;
    }

    // Listen for incoming connections
    //: listen returns fd
    if (listen(seederSocket, 5) == -1) {
        cerr << "Error listening for connections" << endl;
        close(seederSocket);
        return 1;
    }

    logger("Seeder is listening on port : " + to_string(clientIpAndPort.second) + "\n");
    
    thread t0(handleCommandsOfClient, seederSocket, trackerIpAndPort.first, trackerIpAndPort.second, clientIpAndPort.second);
    t0.detach();
    
    while(true){

        // Accept incoming connections
        struct sockaddr_in leecherAddr;
        socklen_t leecherAddrSize = sizeof(leecherAddr);
        int leecherSocket = accept(seederSocket, (struct sockaddr*)&leecherAddr, &leecherAddrSize);
        if (leecherSocket == -1) {
            cerr << "Error accepting client connection" << endl;
            // close(seederSocket);
            return 1;
        }
        thread t1(handleLeecherRequest, leecherSocket);
        t1.detach();

    }

    return 0;
}