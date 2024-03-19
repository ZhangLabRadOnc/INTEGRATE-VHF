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

using namespace std;

#include <htslib/faidx.h>

class Reference {
  private:
    string filePath;
    faidx_t *f;
    map<int, char *> sequences;

  public:
    Reference(string filePath);
    virtual ~Reference();
    string getSeqName(int i);
    int getSeqLength(int i);
    int getSeqCount();
    char *getSeq(int i);
};
