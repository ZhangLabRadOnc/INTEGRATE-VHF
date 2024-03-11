/*
 * Updator.h
 *
 *  Created on: Jun 26, 2016
 *      Author: jinzhang
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "Gene.h"
#include "MyTypes.h"
#include "Reference.h"

class Updator {

  public:
    Updator(){};
    ~Updator(){};

    int update(int gid5p, int gid3p, uint32_t &pos5p, uint32_t &pos3p, split_rna_t &st, Reference &ref, Gene &g);
};
