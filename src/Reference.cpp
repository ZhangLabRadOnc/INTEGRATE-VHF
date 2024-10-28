/*
 * Reference.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: jinzhang
 *      Edit and change: Jan 29, 2013
 */

#include <cstring>
#include <cmath>
#include <htslib/faidx.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "GffHelper.h"
#include "Util.h"
#include "VirusLoader.h"
#include "Reference.h"

using namespace std;

const string Reference::seqNameList[25] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
    "21", "22", "X", "Y", "MT"};

Reference::Reference(string filePath) {
    faidx_t *f = fai_load(filePath.c_str());
    this->faSet.insert(f);
    int n = faidx_nseq(f);
    
    for (int i = 0; i < n; i++) {
        const string originalName = string(faidx_iseq(f, i));
        string mappedName = getStdChrName(originalName);
        bool found = false;
        for (const string &name : seqNameList) {
            if (mappedName.compare(name) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }
        this->nameOriginalToId[originalName] = this->refItems.size();
        this->nameMappedToId[mappedName] = this->refItems.size();
        int seqLength = faidx_seq_len(f, originalName.c_str());
        this->refItems[this->refItems.size()] = RefItem(originalName, mappedName, f, seqLength);
        if (originalName.compare(mappedName) != 0) {
            cout << originalName << " => " << mappedName << endl;
        } else {
            cout << originalName << endl;
        }
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

string Reference::getSeqOriginalName(const int id) const {
    if (id < 0 || id >= this->seqCount) {
        return "";
    }

    auto it = this->refItems.find(id);
    if (it == this->refItems.end()) {
        return "";
    }

    return it->second.originalName;
}

int Reference::getSeqIdByOriginalName(const string &name) const {
    if (this->nameOriginalToId.find(name) == this->nameOriginalToId.end()) {
        return -1;
    }

    auto it = this->nameOriginalToId.find(name);
    if (it == this->nameOriginalToId.end()) {
        return -1;
    }

    return it->second;
}

int Reference::getSeqIdByMappedName(const string &name) const {
    if (this->nameMappedToId.find(name) == this->nameMappedToId.end()) {
        return -1;
    }

    auto it = this->nameMappedToId.find(name);
    if (it == this->nameMappedToId.end()) {
        return -1;
    }

    return it->second;
}

int Reference::getSeqLength(int id) const {
    if (id < 0 || id >= this->seqCount) {
        cerr << "Invalid seq id: " << id << endl;
        return -1;
    }

    auto it = this->refItems.find(id);
    if (it == this->refItems.end()) {
        return -1;
    }

    return it->second.seqLength;
}

int Reference::getSeqCount() const {
    return this->seqCount;
}

bool Reference::isSeqVirus(const int id) const {
    if (id < 0 || id >= this->seqCount) {
        cerr << "Invalid seq id: " << id << endl;
        return -1;
    }

    auto it = this->refItems.find(id);
    if (it == this->refItems.end()) {
        return false;
    }

    return it->second.isVirus;
}

const char *Reference::getSeq(int id) {
    RefItem item = this->refItems[id];
    char *result;
    auto it = this->seqs.find(id);
    if (it != this->seqs.end()) {
        result = it->second;
    } else {
        if (item.f == nullptr) {
            cerr << "No such sequence: " << item.originalName << endl;
            exit(EXIT_FAILURE);
        }
        int fetched;
        result = faidx_fetch_seq(item.f, item.originalName.c_str(), 0, item.seqLength - 1, &fetched);
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

void Reference::readVirusLoaderFA(const VirusLoader &virusLoader) {
    for (const auto &v : virusLoader.selRefs) {
        string filePath = get<0>(v);
        string filePathLower = lowerString(filePath);
        if (filePathLower.ends_with(".fa") || filePathLower.ends_with(".fasta")) {
            faidx_t *f = fai_load(filePath.c_str());
            this->faSet.insert(f);
            const string &originalName = get<1>(v);
            const string &mappedName = get<2>(v);
            this->nameOriginalToId[originalName] = this->refItems.size();
            this->nameMappedToId[originalName] = this->refItems.size();
            int seqLength = faidx_seq_len(f, originalName.c_str());
            this->refItems[this->refItems.size()] = RefItem(originalName, mappedName, f, seqLength, true);
            this->seqCount++;
        } else if (filePathLower.ends_with(".gff")) {
            continue;
        } else {
            cerr << "Unsupported reference file: " << filePath << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void Reference::readVirusLoaderGff(const GffFile &gffFile, const char *pfs, const string &originalName, const string &mappedName, VirusLoader &vl) {
    for (const FaSequenceIndex &seqIdx : gffFile.seqIndices) {
        if (originalName.compare(seqIdx.seqName) != 0) {
            continue;
        }
        string trueName;
        bool found = false;
        for (const auto &v : vl.virusMap) {
            if (v.second.originalName.compare(mappedName) == 0) {
                trueName = v.first;
                found = true;
                break;
            }
        }
        if (!found) {
            continue;
        }
        const char *pfas = pfs + gffFile.faOffset + seqIdx.offset;
        char *seq = (char *)malloc(seqIdx.seqLength + 1);
        seq[seqIdx.seqLength] = '\0';
        int numLines = (int)ceil((double)seqIdx.seqLength / seqIdx.lineBasesLength);
        for (int i = 0; i < numLines; i++) {
            if (i != numLines - 1) {
                memcpy(seq + i * seqIdx.lineBasesLength, pfas + (seqIdx.lineBasesLength + seqIdx.newlineSize) * i, seqIdx.lineBasesLength);
            } else {
                memcpy(seq + i * seqIdx.lineBasesLength, pfas + (seqIdx.lineBasesLength + seqIdx.newlineSize) * i, seqIdx.seqLength - seqIdx.lineBasesLength * i);
            }
        }
        this->nameOriginalToId[mappedName] = this->refItems.size();
        this->nameMappedToId[trueName] = this->refItems.size();
        this->refItems[this->refItems.size()] = RefItem(mappedName, trueName, nullptr, seqIdx.seqLength, true);
        this->seqCount++;
        this->seqs[this->seqCount - 1] = seq;
        break;
    }
}
