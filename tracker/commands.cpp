#include "headers.h"

string executeCommand(vector<string> tokens, int client_socket, string clientIp, int clientPort){
    if(tokens.size() == 0){
        return "Null Char inserted\n";
    }

    if(tokens[0] == "create_user"){
        if(tokens.size() != 3){
            return "Invalid Arguments\nactual command is 'create_user <user_id> <password>'\n";
        }
        string user_id = tokens[1];
        string password = tokens[2];

        return createUser(user_id, password);
    }
    if(tokens[0] == "login"){
        if(tokens.size() != 4){
            return "Invalid Arguments\nactual command is 'login <user_id> <password> <port>'\n";
        }
        string user_id = tokens[1];
        string password = tokens[2];
        int port = stoi(tokens[3]);

        return login(user_id, password, client_socket, clientIp, port);
    }
    if(tokens[0] == "logout"){
        if(tokens.size() != 1){
            return "Invalid Arguments\nactual command is 'logout'\n";
        }

        return logout(client_socket);
    }
    if(tokens[0] == "create_group"){
        if(tokens.size() != 2){
            return "Invalid Arguments\nactual command is 'create_group <group_id>'\n";
        }
        string group_id = tokens[1];

        return createGroup(client_socket, group_id);
    }
    if(tokens[0] == "join_group"){
        if(tokens.size() != 2){
            return "Invalid Arguments\nactual command is 'join_group <group_id>'\n";
        }
        string group_id = tokens[1];

        return joinGroup(client_socket, group_id);
    }
    if(tokens[0] == "leave_group"){
        if(tokens.size() != 2){
            return "Invalid Arguments\nactual command is 'leave_group <group_id>'\n";
        }
        string group_id = tokens[1];

        return leaveGroup(client_socket, group_id);
    }
    if(tokens[0] == "list_requests"){
        if(tokens.size() != 2){
            return "Invalid Arguments\nactual command is 'list_requests <group_id>'\n";
        }
        string group_id = tokens[1];

        return listRequests(client_socket, group_id);
    }
    if(tokens[0] == "accept_request"){
        if(tokens.size() != 3){
            return "Invalid Arguments\nactual command is 'accept_request <group_id> <user_id>'\n";
        }
        string group_id = tokens[1];
        string user_id = tokens[2];

        return acceptRequest(client_socket, group_id, user_id);
    }
    if(tokens[0] == "list_groups"){
        if(tokens.size() != 1){
            return "Invalid Arguments\nactual command is 'list_groups'\n";
        }

        return listGroups(client_socket);
    }
    if(tokens[0] == "list_files"){
        if(tokens.size() != 2){
            return "Invalid Arguments\nactual command is 'list_files <group_id>'\n";
        }
        string group_id = tokens[1];

        return listFiles(client_socket, group_id);;
    }
    if(tokens[0] == "upload_file"){
        cout << "SIze : " << tokens.size();

        cout << "tokens : ";
        for(auto it: tokens){
            cout << it << " ";
        }
        cout << endl;
        if(tokens.size() != 5){
            return "Invalid Arguments\nactual command is 'upload_file <group_id> <file_name> <sha> <bytes>'\n";
        }
        string group_id = tokens[1];
        string file_name = tokens[2];
        string SHA = tokens[3];
        int bytes = stoi(tokens[4]);

        return uploadFilesToGroup(client_socket, group_id, file_name, SHA, bytes);
    }
    if(tokens[0] == "download_file"){
        if(tokens.size() != 3){
            return "downloadFileError: Invalid Arguments\nactual command is 'download_file <group_id> <file_name>'\n";
        }
        string group_id = tokens[1];
        string file_name = tokens[2];

        return downloadFile(client_socket, group_id, file_name);
    }
    if(tokens[0] == "exit") {
        if(tokens.size() != 1){
            return "Invalid Arguments\nactual command is 'exit'\n";
        }
        logout(client_socket);
        return "exit";
    }

    return "invalid command\n";
}

string createUser(string userId, string password){

    if(registeredUsers.find(userId) != registeredUsers.end()){
        return "User already exists!!\n";
    }

    User newUser(userId, password);

    registeredUsers.insert({userId, newUser});

    printRegisteredUserMap();

    return "User created successfully\n";

}

string login(string userId, string password, int client_socket, string clientIp, int clientPort){

    if(socketToUser.find(client_socket) != socketToUser.end()){
        return "You already logged in!! Logout first to login again with different credentials\n";
    }


    auto it = registeredUsers.find(userId);
    if(it == registeredUsers.end()){
        return "User not exists!! Create a user first!!\n";
    }

    if(it->second.password != password){
        return "Wrong password!!\n";
    }

    if(it->second.isLoggedIn == true){
        return "You already logged in from other system!!\n";
    }

    it->second.isLoggedIn = true;
    it->second.ip = clientIp;
    it->second.port = clientPort;

    socketToUser.insert({client_socket, userId});

    printSocketToUserMap();
    printRegisteredUserMap();

    return "logged in successfully\n";

}

string logout(int client_socket){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    auto it2 = registeredUsers.find(userId);

    if(it2->second.groupId != "") removeFilesOfUserFromGroup(userId);

    it2->second.isLoggedIn = false;
    it2->second.ip = "";
    it2->second.port = -1;

    socketToUser.erase(it1);
    

    printSocketToUserMap();
    printRegisteredUserMap();


    return "Logged out successfully\n";

}

string createGroup(int client_socket, string groupId){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    auto it2 = groups.find(groupId);

    if(it2 != groups.end()){
        return "Group with given group_id already exists\n";
    }

    Group newGroup(groupId, userId);

    groups.insert({groupId, newGroup});

    printGroupsMap();

    return "Group created successfully\n";
}

string joinGroup(int client_socket, string groupId){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    string tempGroupId = registeredUsers.at(userId).groupId;

    if(tempGroupId != ""){
        return "You are a part of " + tempGroupId + " as of now\nLeave that group first to join in another group\n";
    }

    auto it2 = groups.find(groupId);

    if(it2 == groups.end()){
        return "Group not exists\n";
    }

    if(it2->second.adminId == userId){
        // admin entry
        registeredUsers.at(userId).groupId = groupId;
        it2->second.participants.insert(userId);

        printRegisteredUserMap();
        printGroupsMap();

        return "Welcome Admin!!\n";
    }

    it2->second.pendingRequests.insert(userId);

    printRegisteredUserMap();
    printGroupsMap();

    return "Join request has been send to admin\n";
    
}

string leaveGroup(int client_socket, string groupId){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    string tempGroupId = registeredUsers.at(userId).groupId;

    if(tempGroupId == ""){
        return "You are not a part of any group\n";
    }

    if(tempGroupId != groupId){
        return "You are not a part of " + groupId + " group\n";
    }

    auto it2 = groups.find(groupId);

    if(it2 == groups.end()){
        return "Group not exists\n";
    }


    it2->second.participants.erase(userId);


    removeFilesOfUserFromGroup(userId);

    registeredUsers.at(userId).groupId = "";

    printRegisteredUserMap();
    printGroupsMap();

    return "You left the group!!\n";

}

string listRequests(int client_socket, string groupId){
    
    auto it1 = socketToUser.find(client_socket);
    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;
    string tempGroupId = registeredUsers.at(userId).groupId;
    if(tempGroupId == ""){
        return "You are not a part of any group\n";
    }

    auto it2 = groups.find(groupId);
    if(it2 == groups.end()){
        return "Group not exists\n";
    }

    if(tempGroupId != groupId){
        return "You are not a part of "+ groupId +" group\n";    
    }

    if(it2->second.adminId != userId){
        return "You are not an Admin!!\n";
    }


    string pendingReqs = "";
    for(auto it: it2->second.pendingRequests){
        pendingReqs += it;
        pendingReqs.push_back(' ');
    }

    printRegisteredUserMap();
    printGroupsMap();

    return "Pending Requests : \n" + pendingReqs;
}

string acceptRequest(int client_socket, string groupId, string userIdOfPending){

    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    string tempGroupId = registeredUsers.at(userId).groupId;

    if(tempGroupId == ""){
        return "You are not a part of any group\n";
    }

    auto it2 = groups.find(groupId);

    if(it2 == groups.end()){
        return "Group not exists\n";
    }

    if(tempGroupId != groupId){
        return "You are not a part of " + groupId + " group\n";
    }

    if(it2->second.adminId != userId){
        return "You are not an Admin!!\n";
    }

    if(it2->second.pendingRequests.find(userIdOfPending) == it2->second.pendingRequests.end()){
        return "User not found in pending list";
    }

    if(registeredUsers.at(userIdOfPending).groupId != ""){
        return "User already joined some other group\nYou can not accept request of user until it leaves another group";
    }

    registeredUsers.at(userIdOfPending).groupId = groupId;
    it2->second.pendingRequests.erase(userIdOfPending);
    it2->second.participants.insert(userIdOfPending);

    printRegisteredUserMap();
    printGroupsMap();

    return "Request accepted successfully\n";

}

string listGroups(int client_socket){

    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    string totalGroups = "";
    for(auto it: groups){
        totalGroups += it.first;
        totalGroups.push_back(' ');
    }

    return "Total Groups : \n" + totalGroups;
}

string listFiles(int client_socket, string groupId){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    string tempGroupId = registeredUsers.at(userId).groupId;

    if(tempGroupId == ""){
        return "You are not a part of any group\n";
    }

    auto it2 = groups.find(groupId);

    if(it2 == groups.end()){
        return "Group not exists\n";
    }

    if(tempGroupId != groupId){
        return "You are not a part of "+ groupId +" group\n";    
    }

    unordered_map <string, File> files = it2->second.filesOfGroup;

    string returnValue = "";
    for(auto it: files){

        returnValue += it.first;
        returnValue.push_back(' ');
    }   
    if(returnValue == ""){
        return "No files are available\n";
    }
    return returnValue;
}


string uploadFilesToGroup(int client_socket, string groupId, string fileName, string SHA, int bytes){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "You are not logged in!!\n";
    }

    string userId = it1->second;

    string tempGroupId = registeredUsers.at(userId).groupId;

    if(tempGroupId == ""){
        return "You are not a part of any group\n";
    }

    auto it2 = groups.find(groupId);

    if(it2 == groups.end()){
        return "Group not exists\n";
    }

    if(tempGroupId != groupId){
        return "You are not a part of "+ groupId +" group\n";    
    }

    User tempUser = registeredUsers.at(userId);

    auto it3 = it2->second.filesOfGroup.find(fileName);

    if(it3 == it2->second.filesOfGroup.end()){
        // Make new File
        string ip = tempUser.ip;
        int port = tempUser.port;
        File newFile(fileName, SHA, bytes);
        newFile.ipAndPorts.insert(ip+":"+to_string(port));
        it2->second.filesOfGroup.insert({fileName, newFile});
        registeredUsers.at(userId).files.insert(fileName);
    }
    else {
        // Append "ip:port" to exitsting File
        string ip = tempUser.ip;
        int port = tempUser.port;
        cout << "Ip : " << ip << " port : " << port << endl;
        cout << "xxx : " << it3->second.fileName << endl;
        it3->second.ipAndPorts.insert(ip+":"+to_string(port));
        registeredUsers.at(userId).files.insert(fileName);
    }
    printRegisteredUserMap();
    printGroupsMap();

    return "Filedata register successfully\n";

}

string downloadFile(int client_socket, string groupId, string fileName){
    auto it1 = socketToUser.find(client_socket);

    if(it1 == socketToUser.end()){
        return "downloadFileError: You are not logged in!!\n";
    }

    string userId = it1->second;

    string tempGroupId = registeredUsers.at(userId).groupId;

    if(tempGroupId == ""){
        return "downloadFileError: You are not a part of any group\n";
    }

    auto it2 = groups.find(groupId);

    if(it2 == groups.end()){
        return "downloadFileError: Group not exists\n";
    }

    if(tempGroupId != groupId){
        return "downloadFileError: You are not a part of "+ groupId +" group\n";    
    }

    auto it3 = it2->second.filesOfGroup.find(fileName);

    if(it3 == it2->second.filesOfGroup.end()){
        return "downloadFileError: File not exist\n";
    }

    File tempFile = it3->second;
    string returnValue = "";
    returnValue += tempFile.SHA;
    returnValue.push_back(' ');
    returnValue += to_string(tempFile.bytes);
    returnValue.push_back(' ');
    for(auto it: tempFile.ipAndPorts){
        returnValue += it;
        returnValue.push_back(' ');
    }

    ///////////////
    User tempUser = registeredUsers.at(userId);
    auto it4 = it2->second.filesOfGroup.find(fileName);
    // Append "ip:port" to exitsting File
    string ip = tempUser.ip;
    int port = tempUser.port;
    it4->second.ipAndPorts.insert(ip+":"+to_string(port));
    registeredUsers.at(userId).files.insert(fileName);
    /////////////////

    printRegisteredUserMap();
    printGroupsMap();

    return returnValue;
}

