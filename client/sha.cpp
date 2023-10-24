// #include <iostream>
// #include <fcntl.h>
// #include <unistd.h>
// #include <cstdio>
// #include <cstdlib>
#include "headers.h"

string findSHA(const char* file_path) {
    // Replace 'your_file_path' with the path to the file you want to hash

    // Open the file using the open system call
    int file_descriptor = open(file_path, O_RDONLY);

    if (file_descriptor < 0) {
        perror("Error opening the file");
        return "";
    }

    // Initialize the SHA-256 context
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[8192];
    ssize_t bytes_read;

    // Read the file in chunks to conserve memory
    while ((bytes_read = read(file_descriptor, buffer, sizeof(buffer))) > 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }

    // Close the file descriptor
    close(file_descriptor);

    // Calculate the SHA-256 hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    char hex_hash[2 * SHA256_DIGEST_LENGTH + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex_hash + 2 * i, "%02x", hash[i]);
    }

    // string str( hash, hash + SHA256_DIGEST_LENGTH );

    // return str;

    return hex_hash;
    // return "ABCD";

}

int giveFileSize(const char *file_path){

    int file_descriptor = open(file_path, O_RDONLY);

    if (file_descriptor == -1) {
        std::cerr << "Error opening file: " << strerror(errno) << std::endl;
        return -1;
    }

    off_t file_size = lseek(file_descriptor, 0, SEEK_END);

    if (file_size == -1) {
        std::cerr << "Error seeking to end of file: " << strerror(errno) << std::endl;
        close(file_descriptor);
        return -1;
    }

    return file_size;
}