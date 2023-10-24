#include "headers.h"

string logFileName;
unordered_map <string, User> registeredUsers;
unordered_map <int, string> socketToUser;
unordered_map <string, Group> groups;


int main(int argc, char *argv[]){
    
    //: Validation of args
    pair<string, int> currentTracker = validateArgs(argc, argv);

    int trackerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (trackerSocket == -1) {
        cerr << "Error creating trackerSocket" << endl;
        return 1;
    }

    logger("TrackerSocket Created\n");

    // Set up server information
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(currentTracker.second);  // Replace with your desired port number
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(trackerSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cerr << "setsockopt";
        exit(EXIT_FAILURE);
    }

    if (inet_pton(AF_INET, currentTracker.first.c_str(), &serverAddr.sin_addr) <= 0){
        cerr << "\nInvalid address/ Address not supported \n";
        return -1;
    }

    // Bind the socket to the specified address and port
    if (bind(trackerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Error binding socket" << endl;
        close(trackerSocket);
        return 1;
    }

    // Listen for incoming connections
    //: listen returns fd
    if (listen(trackerSocket, 5) == -1) {
        cerr << "Error listening for connections" << endl;
        close(trackerSocket);
        return 1;
    }

    logger("Tracker is listening on port : " + to_string(currentTracker.second) + "\n");
    
    thread t0(handleQuitOfTracker, trackerSocket);
    t0.detach();
    
    while(true){

        // Accept incoming connections
        struct sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(trackerSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == -1) {
            cerr << "Error accepting client connection" << endl;
            // close(trackerSocket);
            return 1;
        }

        // Get the IP address and port of the client
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        

        thread t1(handleClientRequest, clientSocket,(string) clientIP, clientPort);
        t1.detach();

    }

    return 0;
}