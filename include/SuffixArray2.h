/*
 * SuffixArray2.h
 *
 *  Created on: May 16, 2013
 *      Author: jinzhang
 */

#pragma once

#include <cstdlib>
#include <iostream>
#include <stdint.h>

using namespace std;

class SuffixArray2 {
  private:
    int *array;

  public:
    SuffixArray2();
    int builtArray(char *tmp, uint32_t length);
    int getSA(int i);
    virtual ~SuffixArray2();
};
