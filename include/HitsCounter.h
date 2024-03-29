/*
 * HitsCounter.h
 *
 *  Created on: Jun 14, 2013
 *      Author: jinzhang
 */

#pragma once

#include <iostream>

#include <sys/stat.h>  // For stat().
#include <sys/types.h> // For stat().

using namespace std;

#include "BWT.h"
#include "Reference.h"
#include "SuffixArray2.h"
#include "Util.h"

class HitsCounter {
  private:
    BWT *bwts;
    BWT *rbwts;
    int number;
    int MIN_BWT_LEN;

  public:
    HitsCounter();
    /*
    int getGenomeBWTF(Reference & ref);
    int getGenomeBWTR(Reference & ref);
    int getCount(char * seq, int len);
     */
    int getNumber() { return number; };
    int allocate(int size);
    int getChromBWTs(Reference &ref, const char *directory);
    int getOne(const char *refseq, uint32_t length, const char *fileName);
    int loadChromBWTs(Reference &ref, const char *directory);
    int loadOne(BWT *bwt, const char *bwtfile);

    int getHitsCount(char *seq, int len);

    virtual ~HitsCounter();

    void setMinBwtLen(int minBwtLen) { MIN_BWT_LEN = minBwtLen; }
};
