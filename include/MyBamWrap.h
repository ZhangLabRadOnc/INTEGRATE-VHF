/*
 * MyBamWrap.h
 *
 *  Created on: Jan 30, 2013
 *      Author: jinzhang
 *  Given a region and a function on each records from the region, apply the function
 */

#pragma once

#include <htslib/sam.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#include "MyTypes.h"

class MyBamWrap {
  private:
    samFile *f;
    sam_hdr_t *h;
    hts_idx_t *idx;

  public:
    MyBamWrap();
    int mysam_open(char *fileName);
    int myGetIndex(char *fileName);
    /*wrap*/
    int myFetchWrap(region_t &region, int (*func)(const bam1_t *b));
    int myPassRegion(region_t &region, const string chrName, int lpos, int rpos);
    virtual ~MyBamWrap();

    void testFetch(char *fileName, string chrName, uint32_t lpos, uint32_t rpos);
};
