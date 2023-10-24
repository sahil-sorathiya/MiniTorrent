#include "headers.h"

void handleClientRequest(int client_socket, string clientIp, int clientPort){

    logger("Client socket number " + to_string(client_socket) + " : connected successfully\n");

    char inputBuffer[1024] = {0};

    while (true){
        memset(inputBuffer, 0, sizeof(inputBuffer));
        if(read(client_socket, inputBuffer, sizeof(inputBuffer)) < 0){
            logger("Client socket number " + to_string(client_socket) + " : Read failed\n");
            close(client_socket);
            return;
        }

        cout << "Client " << client_socket << " says: "<< inputBuffer << endl;

        string inputBufferStr = inputBuffer;
        // //: -2 because of two extra character "end of line" and 
        // //: "null" at the end of string while converted from char [] array inputBuffer
        // inputBufferStr = inputBufferStr.substr(0, inputBufferStr.size() - 2);
        vector <string> tokens = tokenization(inputBufferStr, ' ');

        string outputBuffer = executeCommand(tokens, client_socket, clientIp, clientPort);
        if(outputBuffer == "exit") outputBuffer = "Bye.\n";

        if(write(client_socket, outputBuffer.c_str(), outputBuffer.size()) < 0){
            logger("Client socket number " + to_string(client_socket) + " : Write failed\n");
            close(client_socket);
            return;
        }

        if(outputBuffer == "Bye.\n"){
            close(client_socket);
            return;
        }

    }

}

void handleQuitOfTracker(int tracker_socket){
    string inputFromTracker;
    while (true)
    {
        cin >> inputFromTracker;
        if(inputFromTracker == "quit"){
            close(tracker_socket);
            cout << "Tracker shut down" << endl;
            logger("TrackerSocket closed.");
            exit(0);
        }
    }
}













