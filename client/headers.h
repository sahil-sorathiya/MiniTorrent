#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <fcntl.h>
#include <thread>
#include <openssl/sha.h>
#include <mutex>

// #define CHUNK_SIZE 524288
// #define CHUNK_SIZE 10240
#define CHUNK_SIZE 32768
#define BUF_SIZE 65536
#define MAX_THREADS_FOR_DOWNLOAD 5

using namespace std;

class File {
public:
    
    File(string filePath,string fileName, string SHA, int bytes);

    void printValue();
    string filePath;
    string fileName;
    string SHA;
    int bytes;
    vector <bool> isChunkAvailable;

};

extern string logFileName;
extern unordered_map <string, File> files;

void logger(string txt);
void clearLog();
pair<pair <string, int>, pair <string, int> > validateArgs(int argc, char *argv[]);
string trimString(string str);
vector <string> tokenization(string input, char delimeter);

void handleCommandsOfClient(int seederSocket, string trackerIp, int trackerPort, int clientPort);
void handleLeecherRequest(int leecherSocket);

string executeCommandOfSeeder(vector <string> tokens, string inputFromSeeder,int clientSocket, int clientPort);
string executeCommandOfLeecher(vector <string> tokens, int leecherSocket);

string uploadFilesToGroup(int clientSocket, bool isUpload, string groupId, string fileName, string SHA, int bytes, string path);
string otherCommands(int clientSocket, string inputFromSeeder);

string exitClient(int clientSocket);
string login(int clientSocket, string userId, string password, int port);


string findSHA(const char* file_path);
int giveFileSize(const char *file_path);
string downloadFile(int clientSocket, string groupId, string fileName, string destPath);
string handleDownload(char outputBuf[BUF_SIZE], string fileName, string filePath);

int giveTotalChunksOfFile(int fileSize);
int giveLastChunkSizeOfFile(int fileSize);
int giveFirstByteNoFromChunkNo(int chunkNo);

void printFilesMap();
void printchunkStatusOfOthersVector(vector <unordered_set <string> > chunkStatusOfOthers);

pair <int, string> pieceSelection(vector <unordered_set <string> > &chunkStatusOfOthers, vector <pair<int,int> > &countOfClientForEachChunk);


void handleDownloadThread(vector <string> ipAndPorts, string fileName, string filePath, int bytes, int totalChunks);


