/*
 * Reference.h
 *
 *  Created on: Oct 13, 2011
 *      Author: jinzhang
 *  Edit and change: Jan 29, 2013
 *  Edit and change: Apr 27, 2013
 */

#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

#include <htslib/faidx.h>

class Reference {
  private:
    string filePath;
    faidx_t *f;
    int seqCount;
    map<int, char *> sequences;
    vector<const char *> names;

  public:
    Reference(string filePath);
    virtual ~Reference();
    string getSeqName(int id);
    int getSeqId(string name);
    int getSeqLength(int id);
    int getSeqCount();
    char *getSeq(int id);
};
