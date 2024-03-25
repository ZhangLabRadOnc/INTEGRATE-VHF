/*
 * TidHandler.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#include "MyBamHeader.h"
#include "TidHandler.h"

TidHandler::TidHandler(Reference *ref) {
    this->ref = ref;
}

TidHandler::~TidHandler() {
    this->ref = nullptr;
}

int TidHandler::setRNAAndRef(MyBamHeader &rnabh) {
    for (int i = 0; i < rnabh.getNumTids(); i++) {
        int tid = ref->getSeqId(rnabh.getChrName(i));
        RNA2Ref.insert(pair<int, int>(i, tid));
        Ref2RNA.insert(pair<int, int>(tid, i));
    }
    return 0;
}

int TidHandler::getRNAFromRef(int tid) {
    map<int, int>::iterator it = Ref2RNA.find(tid);
    if (it == Ref2RNA.end())
        return -1;
    else
        return it->second;
}

int TidHandler::getRefFromRNA(int rnaTid) {
    map<int, int>::iterator it = RNA2Ref.find(rnaTid);
    if (it == RNA2Ref.end())
        return -1;
    else
        return it->second;
}

int TidHandler::setDNAAndRef(MyBamHeader &dnabh) {
    for (int i = 0; i < dnabh.getNumTids(); i++) {
        int tid = ref->getSeqId(dnabh.getChrName(i));
        DNA2Ref.insert(pair<int, int>(i, tid));
        Ref2DNA.insert(pair<int, int>(tid, i));
    }
    return 0;
}

int TidHandler::getDNAFromRef(int tid) {
    map<int, int>::iterator it = Ref2DNA.find(tid);
    if (it == Ref2DNA.end())
        return -1;
    else
        return it->second;
}

int TidHandler::getRefFromDNA(int dnaTid) {
    map<int, int>::iterator it = DNA2Ref.find(dnaTid);
    if (it == DNA2Ref.end())
        return -1;
    else
        return it->second;
}
