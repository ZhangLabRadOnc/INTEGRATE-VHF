/*
 * Util.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <htslib/sam.h>
#include <sys/mman.h>
#include <unistd.h>
#include "Util.h"

using namespace std;
namespace fs = filesystem;

string trimString(const string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos)
    {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

vector<string> splitString(const string &str, const string &delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos)
        {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);
        if (!token.empty())
        {
            tokens.push_back(token);
        }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

string replaceString(const string &str, const string &from, const string &to) {
    string result = str;
    size_t startPos = 0;
    while ((startPos = result.find(from, startPos)) != string::npos) {
        result.replace(startPos, from.length(), to);
        startPos += to.length();
    }

    return result;
}

string lowerString(const string &str) {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        result[i] = tolower(result[i]);
    }

    return result;
}

size_t getFileSize(fs::path filePath) {
    try {
        if (fs::exists(filePath)) {
            return fs::file_size(filePath);
        } else {
            throw runtime_error("File does not exist");
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw runtime_error(e);
    }
}

const char* openFileForRead(string fileName, int &fd, size_t &length) {
    if ((fd = open(fileName.c_str(), O_RDONLY)) == -1)
    {
        throw runtime_error("Cannot open file to read: " + fileName);
    }

    length = getFileSize(fileName);
    const char* p = static_cast<const char*>(mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0));

    return p;
}

void closeFileForRead(const char* p, const int fd, const size_t length) {
    munmap((void*)p, length);
    close(fd);
}

string getSeqFromBam(const bam1_t *b) {
    string seq;
    seq.reserve(b->core.l_qseq);
    for (int i = 0; i < b->core.l_qseq; i++) {
        int reada = bam_seqi(bam_get_seq(b), i);
        seq += getCharA(reada);
    }
    return seq;
}

void getRevCompSeq(string &seq) {
    size_t length = seq.length();
    for (size_t i = 0; i < length / 2; i++) {
        char tmp = seq[i];
        seq[i] = getCharComp(seq[length - i - 1]);
        seq[length - i - 1] = getCharComp(tmp);
    }

    if (length % 2 == 1) {
        seq[length / 2] = getCharComp(seq[length / 2]);
    }
}

void getRevCompSeq(vector<char> &seq) {
    string seqStr(seq.begin(), seq.end());
    getRevCompSeq(seqStr);
    seq.assign(seqStr.begin(), seqStr.end());
}

int countMismatches(const string& str1, const string& str2) {
    if (str1.length() != str2.length()) {
        return -1;
    }

    int mismatchCount = 0;
    
    for (size_t i = 0; i < str1.length(); ++i) {
        if (str1[i] != str2[i]) {
            ++mismatchCount;
        }
    }

    return mismatchCount;
}

char *getCStringFromVector(const vector<char> &vec) {
    char *cstr = new char[vec.size() + 1];
    for (size_t i = 0; i < vec.size(); i++) {
        cstr[i] = vec[i];
    }
    cstr[vec.size()] = '\0';
    return cstr;
}
