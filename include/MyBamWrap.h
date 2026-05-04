/*
 * MyBamWrap.h
 *
 *  Created on: Jan 30, 2013
 *      Author: jinzhang
 *  Given a region and a function on each records from the region, apply the function
 */

#pragma once

#include <functional>
#include <htslib/sam.h>
#include <string>
#include "MyTypes.h"

using namespace std;

class MyBamWrap {
    private:
        samFile *f;
        sam_hdr_t *h;
        hts_idx_t *idx;

    public:
        MyBamWrap();
        virtual ~MyBamWrap();
        void mySamOpen(const char *fileName);
        void myLoadIndex(const char *fileName);
        void myFetchWrap(region_t &region, const function<void(const bam1_t *)> &func) const;
        sam_hdr_t* getHeader() const { return h; }
        // int myPassRegion(region_t &region, const string chrName, int lpos, int rpos);
        // void testFetch(const char *fileName, string chrName, uint32_t lpos, uint32_t rpos);
};
