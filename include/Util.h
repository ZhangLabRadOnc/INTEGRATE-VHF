/*
 * Util.h
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#pragma once

#include <cstdio>
#include <filesystem>
#include <htslib/sam.h>
#include <string>
#include <regex>

using namespace std;
namespace fs = std::filesystem;

string trimString(const string &str);
vector<string> splitString(const string &str, const string &delim);
string replaceString(const string &str, const string &from, const string &to);
string lowerString(const string &str);

size_t getFileSize(fs::path filePath);

const char* openFileForRead(string fileName, int &fd, size_t &length);

void closeFileForRead(const char* p, const int fd, const size_t length);

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

inline string getStdChrName(string name) {
    if (name.starts_with("chr") || name.starts_with("Chr")) {
        name = name.substr(3);
    }

    if (name.compare("M") == 0) {
        return "MT";
    }

    return name;
}

string getSeqFromBam(const bam1_t *b);

void getRevCompSeq(string &seq);

void getRevCompSeq(vector<char> &seq);

int countMismatches(const string &str1, const string &str2);
