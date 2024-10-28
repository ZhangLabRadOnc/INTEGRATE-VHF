/*
 * MyBamWrap.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: jinzhang
 */

#include <functional>
#include <iostream>
#include <htslib/sam.h>
#include "MyBamWrap.h"

using namespace std;

MyBamWrap::MyBamWrap() {
    f = nullptr;
    h = nullptr;
    idx = nullptr;
}

MyBamWrap::~MyBamWrap()
{
    if (idx != nullptr) {
        hts_idx_destroy(idx);
        idx = nullptr;
    }
    if (h != nullptr) {
        sam_hdr_destroy(h);
        h = nullptr;
    }
    if (f != nullptr) {
        sam_close(f);
        f = nullptr;
    }
}

void MyBamWrap::mySamOpen(const char *fileName) {
    this->f = sam_open(fileName, "rb");
    if (this->f == nullptr) {
        cerr << "Failed to open file: " << fileName << endl;
        exit(EXIT_FAILURE);
    }
    this->h = sam_hdr_read(f);
    if (this->h == nullptr) {
        sam_close(this->f);
        cerr << "Failed to get header of file: " << fileName << endl;
        exit(EXIT_FAILURE);
    }
}

void MyBamWrap::myLoadIndex(const char *fileName) {
    this->idx = bam_index_load(fileName);
    if (this->idx == nullptr) {
        cerr << "BAM indexing file is not available for " << fileName << endl;
        exit(EXIT_FAILURE);
    }
}

void MyBamWrap::myFetchWrap(region_t &region, const function<void(const bam1_t *)> &func) const {
    int ret;
    hts_itr_t *iter = sam_itr_queryi(this->idx, region.tid, region.lpos, region.rpos);
    if (iter == nullptr) {
        cerr << "Failed to fetch region: " << region.tid << ":" << region.lpos << "-" << region.rpos << "." << endl;
        exit(EXIT_FAILURE);
    }
    bam1_t *b = bam_init1();
    if (b == nullptr) {
        cerr << "Failed to initialize memory." << endl;
        exit(EXIT_FAILURE);
    }
    while ((ret = sam_itr_next(this->f, iter, b)) >= 0) {
        func(b);
    }
    bam_destroy1(b);
    sam_itr_destroy(iter);
}

// int MyBamWrap::myPassRegion(region_t &region, const string chrName, int lpos, int rpos)
// {
//     string regionStr = chrName + ":" + to_string(lpos) + "-" + to_string(rpos);
//     const char *result = hts_parse_reg(regionStr.c_str(), &region.lpos, &region.rpos);
//     if (result == nullptr)
//     {
//         cerr << "Failed to parse region: " << regionStr << endl;
//         return -1;
//     }
//     region.tid = bam_name2id(h, regionStr.c_str());
//     return 0;
// }

// static int fetch_func_test(const bam1_t *b) {
//     char tag[100] = "RG";
//     cout << bam_aux2Z(bam_aux_get(b, tag)) << endl;
//     return 0;
// }

// void MyBamWrap::testFetch(const char *fileName, string chrName, uint32_t lpos, uint32_t rpos) {
//     mySamOpen(fileName);
//     myGetIndex(fileName);
//     region_t region;
//     myPassRegion(region, chrName, lpos, rpos);
//     myFetchWrap(region, fetch_func_test);
// }
