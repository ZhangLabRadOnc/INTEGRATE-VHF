/*
 * Util.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#include <cstdio>
#include <cstring>
#include <filesystem>
#include "Util.h"

using namespace std;
namespace fs = filesystem;

uintmax_t getFileSize(fs::path filePath) {
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

/*
 * Read a block of chars from file till a \n of file
 * return the actual length;
 *
 */
int readBlock(char *block, int length, FILE *infile) {
    fread(block, 1, length, infile);
    if (block[length - 1] != '\n' && block[length - 1] != EOF) {
        fgets(block + length, 1024, infile);
        length += strlen(block + length);
    }

    return length;
}
