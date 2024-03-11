/*
 * Artifact1.h
 *
 *  Created on: Sep 21, 2013
 *      Author: jinzhang
 */

#pragma once

#include <iostream>

using namespace std;

#include "Alignment.h"
#include "Gene.h"
#include "MyTypes.h"

class Artifact1 {
  public:
    Artifact1();
    virtual ~Artifact1();

    bool isAf1(Gene &g, split_rna_t &st, myFind2 &mf2);
};
