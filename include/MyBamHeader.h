/*
 * MyBamHeader.h
 *
 *  Created on: Feb 4, 2013
 *      Author: jinzhang
 */

#pragma once

#include <htslib/sam.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

// also need chr <--> number translation

using namespace std;

class MyBamHeader {
  private:
    map<string, int> rg;
    map<string, int> std;

    samFile *f;
    sam_hdr_t *h;
    int isRG;
    int mInsert;
    int mStd;
    int maxDistance;
    int numTids;

  public:
    map<string, int> tidM;

    MyBamHeader();
    int myBamOpen(const char *fileName);
    int getRGs();
    int getRGStd(int std);
    int getPI(char *rgp);
    int getStd(char *rgp);
    int setTidM();
    int getTid(string &chrName);
    // int run(const char * fileName);
    int run2(const char *fileName);
    string getChrName(int tid);
    int computeMax();

    virtual ~MyBamHeader();

    int getRGSize() { return rg.size(); }

    int getIsRg() const { return isRG; }

    void setIsRg(int isRg) { isRG = isRg; }

    int getMInsert() const { return mInsert; }

    void setMInsert(int insert) { mInsert = insert; }

    void setMStd(int std) { mStd = std; }

    int getMStd() const { return mStd; }

    int getMax() const { return maxDistance; }

    int getNumTids() const { return numTids; }

    void setNumTids(int numTids) { this->numTids = numTids; }

    int getInsertStdFromBAM(const char *filename);
    int printRGs();
};
