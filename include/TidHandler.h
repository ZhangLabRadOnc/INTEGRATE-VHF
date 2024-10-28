/*
 * TidHandler.h
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 *  INTEGARE allows to use bams mapped to different genome files
 *  (i.e. order and names (with chr or without chr) of chr1-22 and x and y can be different).
 *  All genome files should be essentially the same as the reference genome given as input to INTEGRATE.
 *  (i.e subversions of hg19 or ncbi37 can be mixed and used by INTEGRATE)
 *  This Class keeps tracks of different BAM files used, and the dictionary to translate a tid used
 *  in a bam to the correspoding unified tid used by INTEGRATE when loading the reference.
 */

#pragma once

#include <unordered_map>
#include "MyBamHeader.h"
#include "Reference.h"

class TidHandler {

    private:
        Reference *ref;
        unordered_map<int, int> DNA2Ref;
        unordered_map<int, int> Ref2DNA;
        unordered_map<int, int> RNA2Ref;
        unordered_map<int, int> Ref2RNA;

    public:
        TidHandler(Reference *ref);
        virtual ~TidHandler();

        int setRNAAndRef(MyBamHeader &rnabh);
        int getRNAFromRef(int tid) const;
        int getRefFromRNA(int tid) const;

        int setDNAAndRef(MyBamHeader &dnabh);
        int getDNAFromRef(int tid) const;
        int getRefFromDNA(int tid) const;
};
