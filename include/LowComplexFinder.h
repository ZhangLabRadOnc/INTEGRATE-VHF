/*
 * LowComplexFinder.h
 *
 *  Created on: Sep 21, 2013
 *      Author: jinzhang
 */

#ifndef LOWCOMPLEXFINDER_H_
#define LOWCOMPLEXFINDER_H_

#include "sam.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class LowComplexFinder {
  public:
    LowComplexFinder();
    virtual ~LowComplexFinder();

    bool isLowComplex(vector<char> &seq);
    bool isLowComplex(bam1_t *b);
};

#endif /* LOWCOMPLEXFINDER_H_ */
