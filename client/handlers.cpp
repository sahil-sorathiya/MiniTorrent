#include "headers.h"

void handleDownloadChunkThread(vector <string> ipAndPorts, string fileName, string filePath, int bytes, int totalChunks, pair <int, string> chunkAndSeeder){
    bool isLastChunk = false;
    if(chunkAndSeeder.first == totalChunks - 1){
        isLastChunk = true;
    }

    cout << "\n\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\n:::::::::::::::::::INIT THREAD FOR CHUNK " + to_string(chunkAndSeeder.first) + " FROM SEEDER " + chunkAndSeeder.second +":::::::::::::::::::::\n";

    vector <string> ipPort = tokenization(chunkAndSeeder.second, ':');
    string ip = ipPort[0];
    int port = stoi(ipPort[1]);


    int seederSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (seederSocket == -1) {
        cerr << "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError creating socket at seeder connect" << endl;
    }


    struct sockaddr_in seederAddr;
    seederAddr.sin_family = AF_INET;
    seederAddr.sin_port = htons(port);  


    const char* serverIP = ip.c_str();
    if (inet_pton(AF_INET, serverIP, &seederAddr.sin_addr) <= 0) {
        cerr << "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError converting IP address of seeder" << endl;
        close(seederSocket);
        return;
    }


    if (connect(seederSocket, (struct sockaddr*)&seederAddr, sizeof(seederAddr)) == -1) {
        cerr << "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError connecting to seeder" << endl;
        close(seederSocket);
        return;
    }


    string command = "give_chunk " + fileName + " " + to_string(chunkAndSeeder.first);

    
    if(write(seederSocket, command.c_str(), command.size()) < 0){
        logger("\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError at write to seeder at gettingChunkInfo\n");
        cerr << "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError at write to seeder at gettingChunkInfo\n";
        close(seederSocket);
        return;
    }


    char outputBuffer[CHUNK_SIZE];
    memset(outputBuffer, 0, sizeof(outputBuffer));


    if(read(seederSocket, outputBuffer, sizeof(outputBuffer)) < 0){
        logger("\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError at read to seeder at gettingChunkInfo\n");
        cerr << "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError at read to seeder at gettingChunkInfo\n";
        close(seederSocket);
        return;
    }


    string response = outputBuffer;
    cout << "\n\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\n:::::::::::::::: RESPONSE START FOR CHUNK :::::::::::::::::\n"
         << response
         << "\n:::::::::::::::: RESPONSE END :::::::::::::::::\n\n";
    vector <string> resTokens = tokenization(response, ' ');


    if(resTokens.empty()){
        close(seederSocket);
        return;
    }

    if(resTokens[0] == "SeederSideError:"){
        cerr << "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError at fetching chunkInfo from " + ip + ":" + to_string(port) + "\n" + outputBuffer << endl;
        close(seederSocket);
        return;
    } 
    else {

        int fd1 = open(filePath.c_str(), O_RDWR, S_IRWXU);
        if(fd1 < 0){
            cerr <<  "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError while opening the file " + filePath + " at writing chunk of download\n";
            close(fd1);
            close(seederSocket);
            return;
        }

        lseek(fd1, giveFirstByteNoFromChunkNo(chunkAndSeeder.first), SEEK_SET);

        if(isLastChunk){
            if(write(fd1, outputBuffer, giveLastChunkSizeOfFile(bytes)) < 0){
                cerr <<  "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError while write the file " + filePath + " at writing chunk of download\n";
                close(fd1);
                close(seederSocket);
                return;
            }
            isLastChunk = false;
        }
        else {
            if(write(fd1, outputBuffer, sizeof(outputBuffer)) < 0){
                cerr <<  "\nAt chunkDownload Thread of "+ fileName +" ||| chunkNo :" + to_string(chunkAndSeeder.first)  + " ||| seederIP :" + chunkAndSeeder.second + "\nError while write the file " + filePath + " at writing chunk of download\n";
                close(fd1);
                close(seederSocket);
                return;
            }
        }
        close(fd1);
        files.at(fileName).isChunkAvailable[chunkAndSeeder.first] = true;
    }
    close(seederSocket);
    return;

}

void handleDownloadThread(vector <string> ipAndPorts, string fileName, string filePath, int bytes, int totalChunks){

    cout << "\n\nAt Thread of "+ fileName +":\n######################################################\n   New thread for Download " + fileName + " is created\n######################################################\n\n";

    vector <unordered_set <string> > chunkStatusOfOthers(totalChunks);

    //: Knowing chunk status of others 
    //: by sending get_chunk_info request to all ipAndPorts
    for(auto it: ipAndPorts){
        vector <string> ipPort = tokenization(it, ':');
        string ip = ipPort[0];
        int port = stoi(ipPort[1]);

        int seederSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (seederSocket == -1) {
            cerr << "\nAt Thread of "+ fileName +":\nError creating socket at seeder connect" << endl;
            return;
        }

        struct sockaddr_in seederAddr;
        seederAddr.sin_family = AF_INET;
        seederAddr.sin_port = htons(port);  
        const char* serverIP = ip.c_str();
        if (inet_pton(AF_INET, serverIP, &seederAddr.sin_addr) <= 0) {
            cerr << "\nAt Thread of "+ fileName +":\nError converting IP address of seeder" << endl;
            close(seederSocket);
            return;
        }

        if (connect(seederSocket, (struct sockaddr*)&seederAddr, sizeof(seederAddr)) == -1) {
            cerr << "\nAt Thread of "+ fileName +":\nError connecting to seeder" << endl;
            close(seederSocket);
            return;
        }

        string command = "get_chunk_info " + fileName;

        if(write(seederSocket, command.c_str(), command.size()) < 0){
            logger("\nAt Thread of "+ fileName +":\nError at write to seeder at gettingChunkInfo\n");
            cerr << "\nAt Thread of "+ fileName +":\nError at write to seeder at gettingChunkInfo\n";
            close(seederSocket);
            return;
        }

        char outputBuffer[BUF_SIZE];
        memset(outputBuffer, 0, sizeof(outputBuffer));
        if(read(seederSocket, outputBuffer, sizeof(outputBuffer)) < 0){
            logger("\nAt Thread of "+ fileName +":\nError at read to seeder at gettingChunkInfo\n");
            cerr << "\nAt Thread of "+ fileName +":\nError at read to seeder at gettingChunkInfo\n";
            close(seederSocket);
            return;
        }

        string response = outputBuffer;

        vector <string> resTokens = tokenization(response, ' ');

        if(resTokens[0] == "SeederSideError:"){
            cerr << "\nAt Thread of "+ fileName +":\nError at fetching chunkInfo from " + ip + ":" + to_string(port) + "\n" + outputBuffer << endl;
            return;
        } 
        else if(resTokens[0] == "SeederSideSuccess:"){

            resTokens.erase(resTokens.begin());
            for(auto chunkStr: resTokens ){
                int chunkNo = stoi(chunkStr);
                chunkStatusOfOthers[chunkNo].insert(it);
            }

        }
        close(seederSocket);
    }

    vector <pair<int, int> > countOfClientForEachChunk; // < count, chunkNo >

    //: Building countOfClientForEachChunk
    bool isFullFileAvailable = true;
    int chunkNo = 0;
    for(auto it: chunkStatusOfOthers){

        countOfClientForEachChunk.push_back({ it.size(), chunkNo});
        if(it.empty()){
            isFullFileAvailable = false;
            break;
        }
        chunkNo++;
    }

    //: Error if whole file is not available in the group
    if(!isFullFileAvailable){
        cerr << "\nAt Thread of "+ fileName +":\nEntire file is not available in the group, download aborted.";
        return;
    }

    //: Create file with all null chars of totalSize
    int fd1 = open(filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC , S_IRWXU);
    if(fd1 < 0){
        cerr << "\nAt Thread of "+ fileName +":\nError while opening the file " + filePath + " at creating full file of download\n";
        return;
    }

    char fullBuff[CHUNK_SIZE] = {0};

    for(int i = 0; i < totalChunks; i++){
        // For last chunk
        if(i == totalChunks - 1){
            int lastChunkSize = giveLastChunkSizeOfFile(bytes);
            if(lastChunkSize == 0){
                lastChunkSize = CHUNK_SIZE;
            }
            cout << "\nAt Thread of "+ fileName +":\nFOR LAST CHUNK : " << lastChunkSize << endl << endl;
            char tempBuff[lastChunkSize] = {0};
            if(write(fd1, tempBuff, giveLastChunkSizeOfFile(bytes)) < 0){
                cerr << "\nAt Thread of "+ fileName +":\nError while wrtie the file " + filePath + " at writing full file with nullChars of download\n";
                return;
            } 
        }
        // For other chunks
        else {
            cout << "\nAt Thread of "+ fileName +":\nFOR OTHER CHUNKS : " << i << endl;
            if(write(fd1, fullBuff, CHUNK_SIZE) < 0){
                cerr <<  "\nAt Thread of "+ fileName +":\nError while wrtie the file " + filePath + " at writing full file with nullChars of download\n";
                return;
            }
        }
    }
    close(fd1);

    //: Main THREADS
    int totalIterations = totalChunks / MAX_THREADS_FOR_DOWNLOAD;
    for(int i = 0; i < totalIterations; i++){
        int availableThreadWindow = MAX_THREADS_FOR_DOWNLOAD;
        vector <thread> downloadChunkThreads;

        while(availableThreadWindow--){
            pair <int, string> chunkAndSeeder = pieceSelection(chunkStatusOfOthers, countOfClientForEachChunk);

            cout << "\nAt Thread of "+ fileName +":\nAfter PS :::::::  " << chunkAndSeeder.first << " " << chunkAndSeeder.second << "\n";
            downloadChunkThreads.push_back(thread(handleDownloadChunkThread, ipAndPorts, fileName, filePath, bytes, totalChunks, chunkAndSeeder));
        }
        for(int i = 0; i < MAX_THREADS_FOR_DOWNLOAD; i++){
            if(downloadChunkThreads[i].joinable()) downloadChunkThreads[i].join();
        }
        cout << "\nAt Thread of "+ fileName +":\n::::::::::::::::::::::::\n:::::::::::::::::::::::::::::::::\n:::::::::::::::::::::::::::::\n::::::::::::::::::::::::::::::\n\n";
    }
    int remainedChunks = totalChunks % MAX_THREADS_FOR_DOWNLOAD;
    vector <thread> downloadChunkThreads;

    int c = remainedChunks;
    while(c--){
        pair <int, string> chunkAndSeeder = pieceSelection(chunkStatusOfOthers, countOfClientForEachChunk);
        cout << "\nAt Thread of "+ fileName +":\nAfter PS :::::::  " << chunkAndSeeder.first << " " << chunkAndSeeder.second << endl;
        downloadChunkThreads.push_back(thread(handleDownloadChunkThread, ipAndPorts, fileName, filePath, bytes, totalChunks, chunkAndSeeder));
    }
    for(int i = 0; i < remainedChunks; i++){
        if(downloadChunkThreads[i].joinable()) downloadChunkThreads[i].join();
    }

    sleep(10);
    string newSHA = findSHA(filePath.c_str());

    cout << "\nAt Thread of "+ fileName +":\nnew SHA : " << newSHA << endl;
    cout << "\nAt Thread of "+ fileName +":\nold SHA : " << files.at(fileName).SHA << endl;
    if(newSHA != files.at(fileName).SHA ){
        cout << "\nAt Thread of "+ fileName +":\nSHA mismatched, downloaded file is corrupted\nRemoving the file " + filePath + "\n";
        // cout << "\nAt Thread of "+ fileName +":\nFile " + filePath + " removed\n";
        // if(unlink(filePath.c_str()) < 0){
        //     cout <<  "\nAt Thread of "+ fileName +":\nError while unlinking/removal of file " + filePath + " at SHA mismatch\n";
        // }
    }
    sleep(10);
    printchunkStatusOfOthersVector(chunkStatusOfOthers);
    printFilesMap();

    cout << "\n\nAt Thread of "+ fileName +":\n######################################################\n thread for download file " + fileName + " is completed and destroyed\n######################################################\n\n";
}

string handleDownload(char outputBuf[BUF_SIZE], string fileName, string filePath){
    string str = (string) outputBuf;
    vector <string> tokens = tokenization(str, ' ');

    if(tokens[0] == "downloadFileError:"){
        return str;
    }

    string SHA = tokens[0];
    int bytes = stoi(tokens[1]);
    tokens.erase(tokens.begin());
    tokens.erase(tokens.begin());
    vector <string> ipAndPorts = tokens;

    //: Download of new File
    File newFile(filePath, fileName, SHA, bytes);

    int totalChunks = giveTotalChunksOfFile(bytes);
    vector <bool> v(totalChunks, false);
    newFile.isChunkAvailable = v;

    files.insert({fileName, newFile});

    printFilesMap();
    cout << endl << endl;
    cout << "######################################################" << endl;
    cout << "Creating separate thread for download the file" + fileName << endl;
    cout << "######################################################" << endl << endl;
    thread downloadFileThread(handleDownloadThread, ipAndPorts, fileName, filePath, bytes, totalChunks);
    downloadFileThread.detach();

    return "";

}

void handleLeecherRequest(int leecherSocket){

    logger("Client socket number " + to_string(leecherSocket) + " : connected successfully\n");

    char inputBuffer[BUF_SIZE];

    memset(inputBuffer, 0, sizeof(inputBuffer));
    int returnValue = read(leecherSocket, inputBuffer, sizeof(inputBuffer));
    if( returnValue < 0){
        logger("Client socket number " + to_string(leecherSocket) + " : Read failed\n");
        close(leecherSocket);
        return;
    }

    cout << "Leecher " << leecherSocket << " says: "<< inputBuffer << endl;

    string inputBufferStr = inputBuffer;
    // //: -2 because of two extra character "end of line" and 
    // //: "null" at the end of string while converted from char [] array inputBuffer
    // inputBufferStr = inputBufferStr.substr(0, inputBufferStr.size() - 2);
    vector <string> tokens = tokenization(inputBufferStr, ' ');

    string outputBuffer = executeCommandOfLeecher(tokens, leecherSocket);

    if(outputBuffer == "exit") outputBuffer = "Bye.\n";

    cout << "response send to leecher..........." << endl << outputBuffer << ".........." << endl;

    if(write(leecherSocket, outputBuffer.c_str(), outputBuffer.size()) < 0){
        logger("Client socket number " + to_string(leecherSocket) + " : Write failed\n");
        close(leecherSocket);
        return;
    }

    close(leecherSocket);

}

void handleCommandsOfClient(int seederSocket, string trackerIp, int trackerPort, int clientPort){

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error creating socket at tracker connect" << endl;
        exit(0);
    }

    struct sockaddr_in trackerAddr;
    trackerAddr.sin_family = AF_INET;
    trackerAddr.sin_port = htons(trackerPort);  
    const char* serverIP = trackerIp.c_str();
    if (inet_pton(AF_INET, serverIP, &trackerAddr.sin_addr) <= 0) {
        cerr << "Error converting IP address of tracker" << endl;
        close(clientSocket);
        exit(0);
    }

    if (connect(clientSocket, (struct sockaddr*)&trackerAddr, sizeof(trackerAddr)) == -1) {
        cerr << "Error connecting to tracker" << endl;
        close(clientSocket);
        exit(0);
    }

    while (true){
        string inputFromSeeder;
        getline(cin, inputFromSeeder);

        vector <string> tokens = tokenization(inputFromSeeder, ' ');

        string outputBuffer = executeCommandOfSeeder(tokens, inputFromSeeder, clientSocket, clientPort);

        if(outputBuffer != "") cout << outputBuffer << endl;
    }
  
}

string executeCommandOfSeeder(vector <string> tokens, string inputFromSeeder,int clientSocket, int clientPort){
    if(tokens.empty()){
        return "";
    }

    if(tokens[0] == "login"){
        if(tokens.size() != 3){
            return "Invalid arguments\nactual command is 'login <user_id> <password>";
        }
        string userId = tokens[1];
        string password = tokens[2];
        int port = clientPort;
        return login(clientSocket, userId, password, port);
    }
    if(tokens[0] == "upload_file"){
        if(tokens.size() != 3){
            return "Invalid Arguments\nactual command is 'upload_file <file_path> <group_id>'\n";
        }
        string file_path = tokens[1];
        string group_id = tokens[2];

        string SHA = findSHA(file_path.c_str());
        int bytes = giveFileSize(file_path.c_str());
        vector <string> pathTokens = tokenization(file_path, '/');
        
        string fileName = pathTokens.back();

        if(SHA == "") return "Error in SHA function\n";
        if(bytes == -1) return "Error in bytes function\n";
        

        return uploadFilesToGroup(clientSocket, true, group_id, fileName, SHA, bytes, file_path);
    }
    if(tokens[0] == "download_file"){
        if(tokens.size() != 4){
            return "Invalid Arguments\nactual command is 'download_file <group_id> <file_name> <destination_path>'\n";
        }
        string groupId = tokens[1];
        string file_name = tokens[2];
        string dest_path = tokens[3];

        return downloadFile(clientSocket, groupId, file_name, dest_path);
    }
    if(tokens[0] == "show_downloads"){
        if(tokens.size() != 1){
            return "Invalid Arguments\nactual command is 'show_downloads'\n";
        }

        // return downloadFile(clientSocket, group_id, file_name);
    }
    if(tokens[0] == "stop_share"){
        if(tokens.size() != 3){
            return "Invalid Arguments\nactual command is 'stop_share <group_id> <file_name>'\n";
        }
        string group_id = tokens[1];
        string file_name = tokens[2];

        // return uploadFilesToGroup(clientSocket, true, group_id, file_name, SHA, bytes);
    }
    if(tokens[0] == "exit"){
        if(tokens.size() != 1){
            return "Invalid Arguments\nactual command is 'exit'";
        }

        return exitClient(clientSocket);
    }


    return otherCommands(clientSocket, inputFromSeeder);

}

string executeCommandOfLeecher(vector <string> tokens, int leecherSocket){
    if(tokens.empty()){
        return "SeederSideError: Empty command\n";
    }

    if(tokens[0] == "get_chunk_info"){
        if(tokens.size() != 2){
            return "SeederSideError: Invalid Arguments\nactual command is 'get_chunk_info <file_name>'\n";
        }
        string fileName = tokens[1];

        auto it1 = files.find(fileName);
        if(it1 == files.end()){
            return "SeederSideSuccess:";
        }

        string returnValue = "SeederSideSuccess: ";
        int count = 0;
        for(auto it: it1->second.isChunkAvailable){
            if(it == true){
                returnValue += to_string(count);
                returnValue.push_back(' ');
            }
            count++;
        }
        return returnValue;
    }

    if(tokens[0] == "give_chunk"){
        if(tokens.size() != 3){
            return "SeederSideError: Invalid Arguments\nactual command is 'give_chunk <file_name> <chunk_numner>'\n";
        }

        string fileName = tokens[1];
        int chunkNo = stoi(tokens[2]);
        int firstByte = giveFirstByteNoFromChunkNo(chunkNo);

        auto it1 = files.find(fileName);
        if(it1 == files.end()){
            return "SeederSideError: FILE NOT FOUND, ASSUMPTION FAILED\n";
        }

        string filePath = it1->second.filePath;

        int fd1 = open(filePath.c_str(), O_RDONLY , S_IRWXU);
        if(fd1 < 0){
            return "SeederSideError: Error while opening the file " + fileName + " for chunk " + to_string(chunkNo) + "\n";
        }

        lseek(fd1, firstByte, SEEK_SET);

        char chunkBuff[CHUNK_SIZE];

        if(read(fd1, chunkBuff, CHUNK_SIZE) < 0){
            return "SeederSideError: Error while read the file " + fileName + " for chunk " + to_string(chunkNo) + "\n";
        }

        close(fd1);

        return chunkBuff;
    }

    return "Invalid Command to seeder\n";
}

string uploadFilesToGroup(int clientSocket, bool isUpload, string groupId, string fileName, string SHA, int bytes, string path){

    // client side work
    File newFile(path, fileName, SHA, bytes);

    int totalChunks = giveTotalChunksOfFile(bytes);

    vector <bool> v(totalChunks, true);
    newFile.isChunkAvailable = v;

    files.insert({fileName, newFile});

    printFilesMap();

    string commandForTracker = "upload_file " + groupId + " " + fileName + " " + SHA + " " + to_string(bytes);
    if(write(clientSocket, commandForTracker.c_str(), commandForTracker.size()) < 0){
        logger("Error at write to tracker at login\n");
        return "Error at write to tracker at login\n";
    }

    // tracker side work
    char outputBuffer[1024];
    memset(outputBuffer, 0, sizeof(outputBuffer));
    if(read(clientSocket, outputBuffer, sizeof(outputBuffer)) < 0){
        logger("Error at read to tracker at login\n");
        return "Error at read to tracker at login\n";
    }

    return outputBuffer;

}

string downloadFile(int clientSocket, string groupId, string fileName, string destPath){
    string commandForTracker = "download_file " + groupId + " " + fileName;
    if(write(clientSocket, commandForTracker.c_str(), commandForTracker.size()) < 0){
        logger("Error at write to tracker at login\n");
        return "Error at write to tracker at login\n";
    }

    char outputBuffer[BUF_SIZE];
    memset(outputBuffer, 0, sizeof(outputBuffer));
    if(read(clientSocket, outputBuffer, sizeof(outputBuffer)) < 0){
        logger("Error at read to tracker at login\n");
        return "Error at read to tracker at login\n";
    }

    return handleDownload(outputBuffer, fileName, destPath);
}

string login(int clientSocket, string userId, string password, int port){

    string command = "login " + userId + " " + password + " " + to_string(port);
    if(write(clientSocket, command.c_str(), command.size()) < 0){
        logger("Error at write to tracker at login\n");
        return "Error at write to tracker at login\n";
    }

    char outputBuffer[1024];
    memset(outputBuffer, 0, sizeof(outputBuffer));
    if(read(clientSocket, outputBuffer, sizeof(outputBuffer)) < 0){
        logger("Error at read to tracker at login\n");
        return "Error at read to tracker at login\n";
    }

    return outputBuffer;

}

string exitClient(int clientSocket){
    string command = "exit";
    if(write(clientSocket, command.c_str(), command.size()) < 0){
        logger("Error at write to tracker at login\n");
        return "Error at write to tracker at login\n";
    }

    char outputBuffer[1024];
    memset(outputBuffer, 0, sizeof(outputBuffer));
    if(read(clientSocket, outputBuffer, sizeof(outputBuffer)) < 0){
        logger("Error at read to tracker at login\n");
        return "Error at read to tracker at login\n";
    }

    
    cout << outputBuffer << endl;
    close(clientSocket);
    exit(0);
}

string otherCommands(int clientSocket, string inputFromSeeder){

    if(write(clientSocket, inputFromSeeder.c_str(), inputFromSeeder.size()) < 0){
        logger("Error at write to tracker at login\n");
        return "Error at write to tracker at login\n";
    }

    char outputBuffer[1024];
    memset(outputBuffer, 0, sizeof(outputBuffer));
    if(read(clientSocket, outputBuffer, sizeof(outputBuffer)) < 0){
        logger("Error at read to tracker at login\n");
        return "Error at read to tracker at login\n";
    }

    return outputBuffer;
}









