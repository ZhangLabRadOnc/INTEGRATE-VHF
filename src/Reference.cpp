/*
 * Reference.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: jinzhang
 *      Edit and change: Jan 29, 2013
 */

#include <cstring>
#include <htslib/faidx.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "VirusLoader.h"
#include "Reference.h"

using namespace std;

Reference::Reference(string filePath) {
    faidx_t *f = fai_load(filePath.c_str());
    this->faSet.insert(f);
    int n = faidx_nseq(f);
    
    for (int i = 0; i < n; i++) {
        string name = string(faidx_iseq(f, i));
        if (name.starts_with("chr")) {
            name = name.substr(3);
        }
        if (name.compare("MT") == 0){
            name = "M";
        }
        bool found = false;
        for (auto &c : CHRS) {
            if (c.compare(name) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }
        RefItem refItem = RefItem(name, name, f);
        this->nameMappedToId[name] = this->refItems.size();
        this->refItems[this->refItems.size()] = refItem;
        cout << name << endl;
        this->seqCount++;
    }
}

Reference::~Reference() {
    for (auto it = this->seqs.begin(); it != this->seqs.end(); it++) {
        free(it->second);
    }
    this->seqs.clear();
    this->nameMappedToId.clear();
    this->refItems.clear();
    for (auto it = this->faSet.begin(); it != this->faSet.end(); it++) {
        if (*it != nullptr) {
            fai_destroy(*it);
        }
    }
    this->faSet.clear();
}

string Reference::getSeqMappedName(int id) {
    if (id < 0 || id >= this->seqCount) {
        return "";
    }

    return this->refItems[id].mappedName;
}

string Reference::getSeqOriginalName(int id) {
    if (id < 0 || id >= this->seqCount) {
        return "";
    }

    return this->refItems[id].originalName;
}

int Reference::getSeqId(const string &name) {
    if (this->nameMappedToId.find(name) == this->nameMappedToId.end()) {
        if (this->nameOriginalToId.find(name) != this->nameOriginalToId.end()) {
            return this->nameOriginalToId[name];
        }
        return -1;
    }
    return this->nameMappedToId[name];
}

int Reference::getSeqLength(int id) {
    if (id < 0 || id >= this->seqCount) {
        cerr << "Invalid seq id: " << id << endl;
        return -1;
    }
    RefItem item = this->refItems[id];
    return faidx_seq_len(item.f, item.originalName.c_str());
}

int Reference::getSeqCount() {
    return this->seqCount;
}

const char *Reference::getSeq(int id) {
    RefItem item = this->refItems[id];
    int seqLen = faidx_seq_len(item.f, item.originalName.c_str());
    int fetched;
    char *result;
    auto it = this->seqs.find(id);
    if (it != this->seqs.end()) {
        result = it->second;
    } else {
        result = faidx_fetch_seq(item.f, item.originalName.c_str(), 0, seqLen - 1, &fetched);
        for (int i = 0; i < fetched; i++) {
            if (result[i] != 'A' && result[i] != 'T' && result[i] != 'C' && result[i] != 'G' && result[i] != 'N') {
                result[i] = 'N';
            }
        }
        result = (char *)realloc(result, fetched + 2);
        result[fetched] = '$';
        result[fetched + 1] = '\0';
        this->seqs[id] = result;
    }
    return result;
}

void Reference::readVirusLoader(const VirusLoader &virusLoader) {
    for (const auto &v : virusLoader.selRefMap) {
        string filePath = v.first;
        faidx_t *f = fai_load(filePath.c_str());
        this->faSet.insert(f);
        for (const auto &n : v.second) {
            RefItem refItem = RefItem(n.originalName, n.mappedName, f);
            this->nameMappedToId[n.mappedName] = this->refItems.size();
            this->refItems[this->refItems.size()] = refItem;
        }
        this->seqCount++;
    }
}
