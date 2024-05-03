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

Reference::Reference(string filePath) {
    faidx_t *f = fai_load(filePath.c_str());
    this->faSet.insert(f);
    int n = faidx_nseq(f);
    
    for (int i = 0; i < n; i++) {
        const string originalName = string(faidx_iseq(f, i));
        string mappedName = getStdChrName(originalName);
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

string Reference::getSeqOriginalName(int id) {
    if (id < 0 || id >= this->seqCount) {
        return "";
    }

    return this->refItems[id].originalName;
}

string Reference::getSeqMappedName(int id) {
    if (id < 0 || id >= this->seqCount) {
        return "";
    }

    return this->refItems[id].mappedName;
}

int Reference::getSeqIdByOriginalName(const string &name) {
    if (this->nameOriginalToId.find(name) == this->nameOriginalToId.end()) {
        return -1;
    }
    return this->nameOriginalToId[name];
}

int Reference::getSeqIdByMappedName(const string &name) {
    if (this->nameMappedToId.find(name) == this->nameMappedToId.end()) {
        return -1;
    }
    return this->nameMappedToId[name];
}

int Reference::getSeqLength(int id) {
    if (id < 0 || id >= this->seqCount) {
        cerr << "Invalid seq id: " << id << endl;
        return -1;
    }

    return this->refItems[id].seqLength;
}

int Reference::getSeqCount() {
    return this->seqCount;
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
    for (const auto &v : virusLoader.selRefMap) {
        string filePath = v.first;
        if (filePath.ends_with(".fa") || filePath.ends_with(".fasta")) {
            faidx_t *f = fai_load(filePath.c_str());
            this->faSet.insert(f);
            for (const auto &n : v.second) {
                this->nameOriginalToId[n.originalName] = this->refItems.size();
                this->nameMappedToId[n.mappedName] = this->refItems.size();
                int seqLength = faidx_seq_len(f, n.originalName.c_str());
                this->refItems[this->refItems.size()] = RefItem(n.originalName, n.mappedName, f, seqLength);
                this->seqCount++;
            }
        } else if (filePath.ends_with(".gff")) {
            continue;
        } else {
            cerr << "Unsupported reference file: " << filePath << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void Reference::readVirusLoaderGff(const GffFile &gffFile, const char *pfs, const vector<VirusLoader::VirusNamePair> &virusNamePairs) {
    for (const FaSequenceIndex &seqIdx : gffFile.seqIndices) {
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
        string originalName = "N/A";
        string mappedName = "N/A";
        for (const VirusLoader::VirusNamePair &v : virusNamePairs) {
            if (seqIdx.seqName.compare(v.originalName) == 0) {
                originalName = v.originalName;
                mappedName = v.mappedName;
                break;
            }
        }
        this->nameOriginalToId[originalName] = this->refItems.size();
        this->nameMappedToId[mappedName] = this->refItems.size();
        this->refItems[this->refItems.size()] = RefItem(originalName, mappedName, nullptr, seqIdx.seqLength);
        this->seqCount++;
        this->seqs[this->seqCount - 1] = seq;
    }
}
