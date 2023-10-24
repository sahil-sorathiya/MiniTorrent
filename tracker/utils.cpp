#include "headers.h"

void clearLog(){
    // cout << logFileName << endl;
    // cout << logFileName.size();
    int fd = open(logFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if(fd == -1){
        cout << "Error while opening or creating log file." << errno << endl;
        exit(0);
    }
    close(fd);
}

void logger(string txt){
    // ofstream log_file("tracker_log.txt", ios_base::out | ios_base::app);
    // log_file << txt << endl;

    //: opening log file
    int fd = open(logFileName.c_str(), O_RDWR | O_APPEND, S_IRWXU);
    if(fd == -1){
        cout << "Error while opening log file." << errno << endl;
        exit(0);
    }

    write(fd, txt.c_str(), txt.size());

    close(fd);

}

pair <string, int> validateArgs(int argc, char *argv[]){
    if(argc != 3){
        cerr << "Invalid Arguments\n<exe> <tracker_info.txt> <tracker_no>" << endl;
        exit(-1);
    }

    char* fileName = argv[1];
    int trackerNo = atoi(argv[2]);

    logFileName = "txts/tracker_" + to_string(trackerNo) + "_log.txt";

    clearLog();

    pair <string, int> trackerInfo1; // <ip, port>
    pair <string, int> trackerInfo2;

    int fd = open(fileName, O_RDONLY);
    if(fd == -1){
        cout << "Error while opening tracker-info file." << endl;
        exit(0);
    }

    char buf[1024];
    read(fd, buf, 1024);
    
    string buff = buf;

    vector <string> trackerInfos = tokenization(buff, '\n');


    trackerInfo1.first = trackerInfos[0];  
    trackerInfo1.second = stoi(trackerInfos[1]);  
    trackerInfo2.first = trackerInfos[2];  
    trackerInfo2.second = stoi(trackerInfos[3]);  

    if(trackerNo == 1){
        return trackerInfo1;
    } else if(trackerNo == 2){
        return trackerInfo2;
    } else {
        cerr << "Invalid tracker number\n";
        exit(-1);
    }
}

void printRegisteredUserMap(){
    cout << "Registered Users:" << endl;
    for(auto it: registeredUsers){
        it.second.printValue();
    }
    cout << endl;
}

void printSocketToUserMap(){
    cout << "Socket to User Map:" << endl; 
    for(auto it: socketToUser){
        cout << "client_socket : "  << setw(3) << it.first << " || userId : " << setw(15) << it.second << endl; 
    }
    cout << endl;
}

void printGroupsMap(){
    cout << "Total Groups:" << endl;
    for(auto it: groups){
        it.second.printValue();
    }
    cout << endl;
}

string trimString(string str) {
    // Initialize considering no trimming required.
    int atstart = 0, atend = str.size()-1;
    // Spaces at start
    for(int i = 0; i < (int) str.size(); i++) {
        if(str[i] != ' ') {
            atstart = i;
            break;
        }
    }
    // Spaced from end
    for(int i = str.size()-1;i>=0;i--) {
        if(str[i] != ' '){
            atend = i;
            break;
        }
    }
    string fistr = "";
    for(int i=atstart;i<=atend;i++) {
        fistr += str[i];
    }
    return fistr;
}

vector <string> tokenization(string input, char delimeter){

    input = trimString(input);

    vector <string> tokens;
    string tempBuffer;
    int n = input.size();



    for(int i = 0; i < n; i++){
        if(input[i] == delimeter){
            tokens.push_back(tempBuffer);
            tempBuffer = "";
        }
        else if(i == n - 1) {
            tempBuffer.push_back(input[i]);
            tokens.push_back(tempBuffer);
        } 
        else {
            tempBuffer.push_back(input[i]);
        }
    }

    return tokens;
}

//: ---------------- Assumption --------------- //
//: 1
//: This function will be called only if user is logged in 
//: So error handling for "logged in or not" is not written here
//: 2
//: User is already joined in any group
//: So error handling for ""
//: ----------------- Working --------------- //
//: It will iterate to all files in User.files
//: For each file, find that file in Group.filesOfGroup
//: in that file check for File.ipAndPorts contains User.ip + ":" + User.port or not
//: if contains remove it
//: else continue
//: after removal check that File.ipAndPorts.size() == 0 or not
//: If 0 then remove that file from Group.filesOfGroup
void removeFilesOfUserFromGroup(string userId){
    User tempUser = registeredUsers.at(userId);

    unordered_set <string> files = tempUser.files;

    string ip = tempUser.ip;
    int port  = tempUser.port;
    string groupId = tempUser.groupId;

    string ipPort = ip + ":" + to_string(port); 

    for(auto it: files){
        string fileName = it;
        auto fileIt = groups.at(groupId).filesOfGroup.find(fileName);
        auto ipPortIt = fileIt->second.ipAndPorts.find(ipPort);


        fileIt->second.ipAndPorts.erase(ipPortIt);

        if(fileIt->second.ipAndPorts.size() == 0){
            groups.at(groupId).filesOfGroup.erase(fileIt);
        }

    }

    registeredUsers.at(userId).files.clear();
    return;
}

