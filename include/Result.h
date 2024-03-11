/*
 * Result.h
 *
 *  Created on: Sep 26, 2013
 *      Author: jinzhang
 */

#pragma once

#include <htslib/sam.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "BreakPoint.h"
#include "FocalRegionHandler.h"
#include "Gene.h"
#include "MyTypes.h"
#include "Reference.h"
#include "Updator.h"
#include "Util.h"

class Result {
  private:
    vector<result_t> results;
    int indi;

  public:
    vector<break_point_record_t> bkvec;

    Result();
    virtual ~Result();

    int addResult(result_t result);
    int searchResult(int geneId1, int geneId2, result_t &result);
    result_t *getOneResult(int index);
    int printOneResult(int index, ofstream &outFile, Reference &ref, int isRunningNormal);
    int printAllResult(const char *filename, Reference &ref, int isRunningNormal);
    int getTiers(double pn);
    int printSummary(const char *filename, Gene &g, int isRunningNormal, int largeNum);
    int checkALLPrime();

    int getSize();

    int getIndi() const { return indi; }

    void setIndi(int indi) { this->indi = indi; }

    int removeMultiple(Gene &g, int largeNum);
    int combineRecord(Gene &g);

    int printExons(const char *filename, Gene &g, Reference &ref, int isRunningNormal, const char *bkfile, const char *bkfileBEDPE, const char *bkfileVCF, const char *refname, const char *sample_name);

    // copy and modified from printExons from 0.1.c and rm //bk and change
    int getAllJunctionsStep1(Gene &g, Reference &ref);
    int getAllJunctionsStep2(const char *filename, Gene &g, Reference &ref);
    int getAllJunctionsStep3(const char *filename, Gene &g, Reference &ref);
    int getAllJunctionsStep4(Gene &g, Reference &ref);
    int getAllJunctionsStep5(Gene &g, Reference &ref);
    int getAllJunctionsStep6(const char *filename, Gene &g, Reference &ref);
};
