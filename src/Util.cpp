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
