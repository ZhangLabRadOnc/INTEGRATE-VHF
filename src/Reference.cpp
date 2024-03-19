/*
 * Reference.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: jinzhang
 *      Edit and change: Jan 29, 2013
 */

#include <htslib/faidx.h>
#include <map>
#include <string>
#include "Reference.h"

Reference::Reference(string filePath) {
    this->filePath = filePath;
    this->f = fai_load(this->filePath.c_str());
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
}

string Reference::getSeqName(int i) {
    return string(faidx_iseq(f, i));
}

int Reference::getSeqLength(int i) {
    return faidx_seq_len(this->f, faidx_iseq(f, i));
}

int Reference::getSeqCount() {
    return faidx_nseq(f);
}

char *Reference::getSeq(int i) {
    int seqLen = faidx_seq_len(this->f, faidx_iseq(f, i));
    int fetched;
    char *result;
    auto it = this->sequences.find(i);
    if (it !=  this->sequences.end()) {
        result = it->second;
    } else {
        result = faidx_fetch_seq(f, faidx_iseq(f, i), 0, seqLen - 1, &fetched);
        for (int i = 0; i < fetched; i++) {
            if (result[i] != 'A' && result[i] != 'T' && result[i] != 'C' && result[i] != 'G' && result[i] != 'N') {
                result[i] = 'N';
            }
        }
        result = (char *)realloc(result, fetched + 2);
        result[fetched] = '$';
        result[fetched + 1] = '\0';
        this->sequences[i] = result;
    }
    return result;
}
