/*
 * Util.h
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#pragma once

#include <cstdio>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

size_t getFileSize(fs::path filePath);

const char* openFileForRead(string fileName, int &fd, size_t &length);

void closeFileForRead(const char* p, const int fd, const size_t length);

int readBlock(char *block, int length, FILE *infile);

inline char getCharComp(char reada) {
    switch (reada) {
        case 'A':
            return 'T';
        case 'C':
            return 'G';
        case 'G':
            return 'C';
        case 'T':
            return 'A';
        case 'N':
            return 'N';
        default:
            return 'N';
    }
}

inline char getCharA(int reada) {
    switch (reada) {
        case 1:
            return 'A';
        case 2:
            return 'C';
        case 4:
            return 'G';
        case 8:
            return 'T';
        case 15:
            return 'N';
        default:
            return 'N';
    }
}
