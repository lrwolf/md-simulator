//
//  Util.cpp
//  sim
//
//  Created by Lucas Wolf on 3/30/14.
//
//

#include "Util.h"

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#define GETCWD ::getcwd

string getCurrentDir() {
    const size_t pathSize = 4096;
    char currentDir[pathSize];
    
    // Check if we received the path
    if (GETCWD(currentDir, pathSize) != NULL) {
        return string(currentDir);
    }
    
    return  string("");
}

bool SDKFile::writeBinaryToFile(const char* fileName, const char* binary, size_t numBytes) {
    FILE *output = NULL;
    output = fopen(fileName, "wb");
    if (output == NULL) {
        return false;
    }
    
    fwrite(binary, sizeof(char), numBytes, output);
    fclose(output);
    
    return true;
}

bool SDKFile::readBinaryFromFile(const char* fileName) {
    FILE * input = NULL;
    input = fopen(fileName, "rb");
    if (input == NULL) {
        return false;
    }
    
    fseek(input, 0L, SEEK_END);

    size_t size = ftell(input);
    rewind(input);

    char* binary = (char*)malloc(size);
    if (binary == NULL) {
        return false;
    }
    fread(binary, sizeof(char), size, input);
    fclose(input);
    
    source_.assign(binary, size);
    free(binary);
    
    return true;
}

bool SDKFile::open(const char* fileName) {
    size_t size;
    char* str;
    
    // Open file stream
    fstream f(fileName, (fstream::in | fstream::binary));
    
    // Check if we have opened file stream
    if (f.is_open()) {
        size_t sizeFile;
        // Find the stream size
        f.seekg(0, fstream::end);
        size = sizeFile = (size_t)f.tellg();
        f.seekg(0, fstream::beg);
        
        str = new char[size + 1];
        if (!str) {
            f.close();
            return  false;
        }
        
        // Read file
        f.read(str, sizeFile);
        f.close();
        str[size] = '\0';
        source_  = str;
        
        delete[] str;
        
        return true;
    }
    
    return false;
}
