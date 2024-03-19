/*
 * Reference.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: jinzhang
 *      Edit and change: Jan 29, 2013
 */

#include <cstring>
#include <htslib/faidx.h>
#include <map>
#include <string>
#include "Reference.h"

Reference::Reference(string filePath) {
    this->filePath = filePath;
    this->f = fai_load(this->filePath.c_str());
    this->seqCount = faidx_nseq(f);
    for (int i = 0; i < this->seqCount; i++) {
        const char *name = faidx_iseq(this->f, i);
        if (strstr(name, "chr") == name) {
            name += 3;
        }
        names.push_back(name);
    }
}

Reference::~Reference() {
    if (this->f != nullptr) {
        fai_destroy(this->f);
        this->f = nullptr;
    }
    for (auto it = this->sequences.begin(); it != this->sequences.end(); it++) {
        free(it->second);
    }
    this->sequences.clear();
    this->names.clear();
}

string Reference::getSeqName(int id) {
    if (id < 0 || id >= this->seqCount) {
        return "";
    }

    return names[id];
}

int Reference::getSeqId(string name) {
    int idx = 0;
    for (const auto &i : names) {
        if (name.compare(i) == 0) {
            return idx;
        }
        idx++;
    }

    return -1;
}

int Reference::getSeqLength(int id) {
    return faidx_seq_len(this->f, faidx_iseq(f, id));
}

int Reference::getSeqCount() {
    return this->seqCount;
}

char *Reference::getSeq(int id) {
    int seqLen = faidx_seq_len(this->f, faidx_iseq(f, id));
    int fetched;
    char *result;
    auto it = this->sequences.find(id);
    if (it !=  this->sequences.end()) {
        result = it->second;
    } else {
        result = faidx_fetch_seq(f, faidx_iseq(f, id), 0, seqLen - 1, &fetched);
        for (int i = 0; i < fetched; i++) {
            if (result[i] != 'A' && result[i] != 'T' && result[i] != 'C' && result[i] != 'G' && result[i] != 'N') {
                result[i] = 'N';
            }
        }
        result = (char *)realloc(result, fetched + 2);
        result[fetched] = '$';
        result[fetched + 1] = '\0';
        this->sequences[id] = result;
    }
    return result;
}
