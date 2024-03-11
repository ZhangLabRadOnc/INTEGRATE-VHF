/*
 * BreakPoint.h
 *
 *  Created on: March 11, 2014
 *      Author: jinzhang
 */

#pragma once

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>

using namespace std;

#include "Alignment.h"
#include "MyTypes.h"
#include "Reference.h"

class BreakPoint {
  public:
    BreakPoint();
    virtual ~BreakPoint();

    int getBreakPoints(vector<break_point_record_t> &bkvec, const char *filename, const char *filename1, const char *filename2, const char *refname, Reference &ref, const char *sample_name);

    int getOneBKRNA(break_point_record_t &bkt, Reference &ref);

    int getOneBKDNA(break_point_record_t &bkt, Reference &ref);

    int printOneBK(break_point_record_t &bkt, Reference &ref, ofstream &outFile);
    int AddOneVCF(break_point_record_t &bkt, Reference &ref);
    int printOneBEDPE(break_point_record_t &bkt, Reference &ref, ofstream &outFile);
};
