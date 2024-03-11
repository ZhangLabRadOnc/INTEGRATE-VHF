/*
 * RunCode.h
 *
 *  Created on: Oct 7, 2013
 *      Author: jinzhang
 */

#pragma once

#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

#include "Dna.h"
#include "FusionGraph.h"
#include "Gene.h"
#include "HitsCounter.h"
#include "MyBamWrap.h"
#include "Reference.h"
#include "Result.h"
#include "Rna.h"
#include "SuffixArray2.h"
#include "TidHandler.h"

class RunCode {
  public:
    RunCode();
    virtual ~RunCode();

    int runBuildBWTs(int argc, char *argv[]);
    int runFindFusions(int argc, char *argv[]);
};
