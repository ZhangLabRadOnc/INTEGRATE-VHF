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

const char* openFileForRead(string fileName, int &fd, size_t &length)
{
    if ((fd = open(fileName.c_str(), O_RDONLY)) == -1)
    {
        throw runtime_error("Cannot open file to read: " + fileName);
    }

    length = getFileSize(fileName);
    const char* p = static_cast<const char*>(mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0));

    return p;
}

void closeFileForRead(const char* p, const int fd, const size_t length)
{
    munmap((void*)p, length);
    close(fd);
}
