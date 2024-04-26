/*
 * HitsCounter.h
 *
 *  Created on: Jun 14, 2013
 *      Author: jinzhang
 */

#pragma once

#include <filesystem>
#include <iostream>
#include "BWT.h"
#include "Reference.h"

using namespace std;
namespace fs = std::filesystem;

class HitsCounter {
  private:
    BWT *bwts;
    BWT *rbwts;
    int number;
    int MIN_BWT_LEN;

  public:
    HitsCounter();
    int getNumber() { return number; };
    int allocate(int size);
    int getChromBWTs(Reference &ref, const char *directory);
    int getOne(const char *refseq, uint32_t length, const fs::path &filePath);
    int loadChromBWTs(Reference &ref, const char *directory);
    int loadOne(BWT *bwt, const fs::path &filePath);

    int getHitsCount(char *seq, int len);

    virtual ~HitsCounter();

    void setMinBwtLen(int minBwtLen) { MIN_BWT_LEN = minBwtLen; }
};
