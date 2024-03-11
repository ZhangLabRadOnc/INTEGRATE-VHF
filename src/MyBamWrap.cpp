/*
 * MyBamWrap.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: jinzhang
 */

#include "MyBamWrap.h"

MyBamWrap::MyBamWrap() {
    f = nullptr;
    h = nullptr;
    idx = nullptr;
}

MyBamWrap::~MyBamWrap()
{
    if (idx != nullptr)
    {
        hts_idx_destroy(idx);
        idx = nullptr;
    }
    if (h != nullptr)
    {
        sam_hdr_destroy(h);
        h = nullptr;
    }
    if (f != nullptr)
    {
        sam_close(f);
        f = nullptr;
    }
}

int MyBamWrap::mysam_open(const char *fileName) {
    f = sam_open(fileName, "rb");
    if (f == nullptr)
    {
        cerr << "Failed to open file: " << fileName << endl;
        exit(1);
    }
    h = sam_hdr_read(f);
    if (h == nullptr)
    {
        sam_close(f);
        cerr << "Failed to get header of file: " << fileName << endl;
        exit(1);
    }

    return 0;
}

int MyBamWrap::myFetchWrap(region_t &region, int (*func)(const bam1_t *b))
{
    int ret;
    hts_itr_t *iter = sam_itr_queryi(idx, region.tid, region.lpos, region.rpos);
    bam1_t *b = bam_init1();
    while ((ret = sam_itr_next(f, iter, b)) >= 0) {
        func(b);
    }
    bam_destroy1(b);
    sam_itr_destroy(iter);
    return (ret == -1) ? 0 : ret;
}

int MyBamWrap::myGetIndex(const char *fileName) {
    idx = bam_index_load(fileName);
    if (idx == 0) {
        cerr << "BAM indexing file is not available for " << fileName << endl;
        exit(1);
    }
    return 0;
}

int MyBamWrap::myPassRegion(region_t &region, const string chrName, int lpos, int rpos)
{
    string regionStr = chrName + ":" + to_string(lpos) + "-" + to_string(rpos);
    const char *result = hts_parse_reg(regionStr.c_str(), &region.lpos, &region.rpos);
    if (result == nullptr)
    {
        cerr << "Failed to parse region: " << regionStr << endl;
        return -1;
    }
    region.tid = bam_name2id(h, regionStr.c_str());
    return 0;
}

static int fetch_func_test(const bam1_t *b) {
    char tag[100] = "RG";
    cout << bam_aux2Z(bam_aux_get(b, tag)) << endl;
    return 0;
}

void MyBamWrap::testFetch(const char *fileName, string chrName, uint32_t lpos, uint32_t rpos) {
    mysam_open(fileName);
    myGetIndex(fileName);
    region_t region;
    myPassRegion(region, chrName, lpos, rpos);
    myFetchWrap(region, fetch_func_test);
}
