/*
 * TidHandler.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#include "MyBamHeader.h"
#include "Reference.h"
#include "Util.h"
#include "TidHandler.h"

TidHandler::TidHandler(Reference *ref) {
    this->ref = ref;
}

TidHandler::~TidHandler() {
    this->ref = nullptr;
}

int TidHandler::setRNAAndRef(MyBamHeader &rnabh) {
    for (int i = 0; i < rnabh.getNumTids(); i++) {
        string name = rnabh.getChrName(i);
        int tid = ref->getSeqIdByOriginalName(name);
        if (tid < 0) {
            name = getStdChrName(name);
            tid = ref->getSeqIdByMappedName(name);
        }
        RNA2Ref.insert(pair<int, int>(i, tid));
        Ref2RNA.insert(pair<int, int>(tid, i));
    }
    return 0;
}

int TidHandler::getRNAFromRef(int tid) const {
    auto it = Ref2RNA.find(tid);
    return it == Ref2RNA.end() ? -1 : it->second;
}

int TidHandler::getRefFromRNA(int tid) const {
    auto it = RNA2Ref.find(tid);
    return it == RNA2Ref.end() ? -1 : it->second;
}

int TidHandler::setDNAAndRef(MyBamHeader &dnabh) {
    for (int i = 0; i < dnabh.getNumTids(); i++) {
        string name = dnabh.getChrName(i);
        int tid = ref->getSeqIdByOriginalName(name);
        if (tid < 0) {
            name = getStdChrName(name);
            tid = ref->getSeqIdByMappedName(name);
        }
        DNA2Ref.insert(pair<int, int>(i, tid));
        Ref2DNA.insert(pair<int, int>(tid, i));
    }
    return 0;
}

int TidHandler::getDNAFromRef(int tid) const {
    auto it = Ref2DNA.find(tid);
    return it == Ref2DNA.end() ? -1 : it->second;
}

int TidHandler::getRefFromDNA(int tid) const {
    auto it = DNA2Ref.find(tid);
    return it == DNA2Ref.end() ? -1 : it->second;
}
