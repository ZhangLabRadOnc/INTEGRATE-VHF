/*
 * Reference.h
 *
 *  Created on: Oct 13, 2011
 *      Author: jinzhang
 *  Edit and change: Jan 29, 2013
 *  Edit and change: Apr 27, 2013
 */

#pragma once

#include <fstream>
#include <htslib/faidx.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "GffHelper.h"
#include "VirusLoader.h"

using namespace std;

class Reference {
  public:
    struct RefItem {
      string originalName;
      string mappedName;
      faidx_t *f = nullptr;
      int seqLength = 0;
      RefItem() = default;
      RefItem(string originalName, string mappedName, faidx_t *f, int seqLength) : originalName(originalName), mappedName(mappedName), f(f), seqLength(seqLength) {}
    };

  private:
    int seqCount = 0;
    unordered_map<int, char *> seqs;
    unordered_set<faidx_t *> faSet;
    unordered_map<int, RefItem> refItems;
    unordered_map<string, int> nameMappedToId;
    unordered_map<string, int> nameOriginalToId;

  public:
    Reference(string filePath);
    virtual ~Reference();
    string getSeqOriginalName(int id);
    int getSeqIdByOriginalName(const string &name);
    int getSeqIdByMappedName(const string &name);
    int getSeqLength(int id);
    int getSeqCount();
    const char *getSeq(int id);
    void readVirusLoaderFA(const VirusLoader &virusLoader);
    void readVirusLoaderGff(const GffFile &gffFile, const char *pfs, const string &originalName, const string &mappedName, VirusLoader &vl);
};
