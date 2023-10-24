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
    //: opening log file
    int fd = open(logFileName.c_str(), O_RDWR | O_APPEND, S_IRWXU);
    if(fd == -1){
        cout << "Error while opening log file." << errno << endl;
        exit(0);
    }

    write(fd, txt.c_str(), txt.size());

    close(fd);

}

pair<pair <string, int>, pair <string, int> > validateArgs(int argc, char *argv[]){
    if(argc != 3){
        cerr << "Invalid Arguments\n<exe> <IP>:<PORT> <tracker_info.txt>" << endl;
        exit(-1);
    }

    string ipAndPortStr = argv[1];
    char* fileName = argv[2];

    logFileName = "txts/client_log.txt";

    clearLog();

    pair<pair <string, int>, pair <string, int> > ipAndPortInfo; // client:<ip, port>, tracker:<ip,port>

    int fd = open(fileName, O_RDONLY);
    if(fd == -1){
        cout << "Error while opening tracker-info file." << endl;
        exit(0);
    }

    char buf[1024];
    read(fd, buf, 1024);
    
    string buff = buf;

    vector <string> clientInfo = tokenization(ipAndPortStr, ':');
    vector <string> trackerInfos = tokenization(buff, '\n');

    ipAndPortInfo.first.first = clientInfo[0];  
    ipAndPortInfo.first.second = stoi(clientInfo[1]); 
    ipAndPortInfo.second.first = trackerInfos[0];  
    ipAndPortInfo.second.second = stoi(trackerInfos[1]);  

    return ipAndPortInfo;

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

int giveTotalChunksOfFile(int fileSize){
    return ceil(fileSize * 1.0 / CHUNK_SIZE);
}

int giveFirstByteNoFromChunkNo(int chunkNo){
    return chunkNo * CHUNK_SIZE;
}

int giveLastChunkSizeOfFile(int fileSize){
    return fileSize % CHUNK_SIZE;
}

void printFilesMap(){
    for(auto it: files){
        cout << endl;
        cout << "FILE_INFO....................." << endl;
        it.second.printValue();
     }
}

void printchunkStatusOfOthersVector(vector <unordered_set <string> > chunkStatusOfOthers){
    int count = 0;
    cout << endl;
    cout << "CHUNK STATUS OF OTHERS........................." << endl;
    for(auto it: chunkStatusOfOthers){
        cout << "Chunk No " << count << " : ";
        for(auto ipPort: it){
            cout << ipPort << " ";
        }
        count++;
        cout << endl;
    }
    cout << endl << endl;
    
}

mutex m;
pair <int, string> pieceSelection(vector <unordered_set <string> > &chunkStatusOfOthers, vector <pair<int,int> > &countOfClientForEachChunk){
    m.lock();
        // printchunkStatusOfOthersVector(chunkStatusOfOthers);

        // cout << "countOfClientForEachChunk : ..........................." << endl;
        // for(auto it: countOfClientForEachChunk){
        //     cout << it.first << ":" << it.second << endl;
        // }
        // cout << endl;

        sort(countOfClientForEachChunk.begin(), countOfClientForEachChunk.end());

        auto it = countOfClientForEachChunk.begin();

        int chunkNo = it->second;
        int totalSeeders = it->first;

        auto r = rand() % totalSeeders;

        auto it2 = chunkStatusOfOthers[chunkNo].begin();

        advance(it2, r);

        string ipPortToReturn = *it2;

        countOfClientForEachChunk.erase(it);

    m.unlock();
    return {chunkNo, ipPortToReturn};

}