/*
 * MyTypes.h
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct transcript_t{
    // int bin;
    string name;
    string chrName;
    int tid;
    int strand;
    uint32_t txStart;
    uint32_t txEnd;
    uint32_t cdsStart; // Dec 7, 2015 start to use cdsStart and cdsEnd for in/out-of-frame and peptide
    uint32_t cdsEnd;
    int exonCount;
    uint32_t *exonStarts = nullptr;
    uint32_t *exonEnds = nullptr;
    uint32_t *exonRealStarts = nullptr;
    uint32_t *exonRealEnds = nullptr;
    // int score;
    string name2;
    bool isVirus = false;
};

typedef struct {
    uint32_t start;
    uint32_t end;
} exon_t;

typedef struct {
    int tid;
    uint32_t start;
    uint32_t end;
    int strand;

    vector<int> transIds;
    vector<int> exonIds;
    int geneId;

} exon_map_t;

struct gene_t{
    int tid;
    int strand;
    uint32_t leftLimit;
    uint32_t rightLimit;
    string chrName;
    string name2;
    vector<int> transIds;

    int fakeId; //-1 for no complex genes at multiple locations. 0-upper means yes. so that you see diff locations of the same gene;

    vector<int> anchors;
    bool isVirus = false;
};

struct encompass_rna_t {
    string name;
    int strand1 = 0;
    int tid1 = -1;
    int pos1 = -1;
    int len1 = -1;
    int geneId1 = -1;
    vector<uint32_t> cigar1;

    int strand2 = -1;
    int tid2 = -1;
    int pos2 = -1;
    int len2 = -1;
    int geneId2 = -1;
    vector<uint32_t> cigar2;

    int numCopy = 0;

    vector<char> seq1;
    vector<char> seq2;
};

typedef struct {
    string name;
    int strand;
    int tid;
    int pos;
    int len;
    vector<uint32_t> cigar;

    int insert;
    int std;

    char rg[256];

    int geneId;

    vector<char> seq;

} anchor_rna_t;

typedef struct {
    string name;
    vector<char> seq;
    int clipped;
    int mtid;
    uint32_t mpos;

} hardclip_t;

typedef struct {
    int strand;
    int a;
    int b;

    int map_case; // 0 read from left, 1 read from right, 2 read contain exon

    int miss;
    int gap;

    int geneId;
    vector<int> transIds;
    vector<int> exonIds;

    int unmapId;

} map_emt_t;

typedef struct {
    string name;
    vector<char> seq;
} unmapped_t;

typedef struct {
    int tid;  // tid of chromosome
    int lpos; // start
    int rpos; // end
} region_t;

typedef struct {
    int tid;
    int lpos;
    int rpos;
    int strand;
} region_to_map_t;

struct split_rna_t {
    string name = "";
    int strand1 = 0;
    int tid1 = 0;
    uint32_t pos1 = 0;
    int len1 = 0;
    int bkLeft1 = 0;

    int isCMap = 0;
    int strandC = 0;
    int tidC = 0;
    uint32_t posC = 0;
    int lenC = 0;

    int strand2 = 0;
    int tid2 = 0;
    uint32_t pos2 = 0;
    int len2 = 0;
    int bkLeft2 = 0;

    int cwith1 = 0;

    int geneId1 = 0;
    int geneId2 = 0;

    int difference1 = 0;
    int difference2 = 0;

    vector<char> seq;

    int clusterId = 0;
    int spId = 0;

    int hits = 0;

    int small = 0;
    bool reversed = false;

    bool operator == (const split_rna_t &other) const {
        return
            name == other.name &&
            strand1 == other.strand1 &&
            tid1 == other.tid1 &&
            pos1 == other.pos1 &&
            len1 == other.len1 &&
            bkLeft1 == other.bkLeft1 &&
            strand2 == other.strand2 &&
            tid2 == other.tid2 &&
            pos2 == other.pos2 &&
            len2 == other.len2 &&
            bkLeft2 == other.bkLeft2 &&
            geneId1 == other.geneId1 &&
            geneId2 == other.geneId2 &&
            seq == other.seq &&
            reversed == other.reversed;
    }

    bool operator < (const split_rna_t &other) const {
        if (name != other.name) return name < other.name;
        if (tid1 != other.tid1) return tid1 < other.tid1;
        if (strand1 != other.strand1) return strand1 < other.strand1;
        if (pos1 != other.pos1) return pos1 < other.pos1;
        if (tid2 != other.tid2) return tid2 < other.tid2;
        if (strand2 != other.strand2) return strand2 < other.strand2;
        if (pos2 != other.pos2) return pos2 < other.pos2;

        return false;
    }

    public:
        split_rna_t() {}
        split_rna_t(const split_rna_t &other) {
            name = other.name;
            strand1 = other.strand1;
            tid1 = other.tid1;
            pos1 = other.pos1;
            len1 = other.len1;
            bkLeft1 = other.bkLeft1;
            strand2 = other.strand2;
            tid2 = other.tid2;
            pos2 = other.pos2;
            len2 = other.len2;
            bkLeft2 = other.bkLeft2;
            cwith1 = other.cwith1;
            geneId1 = other.geneId1;
            geneId2 = other.geneId2;
            seq = other.seq;
            clusterId = other.clusterId;
            spId = other.spId;
            hits = other.hits;
            small = other.small;
            reversed = other.reversed;
        }
};

typedef struct {
    int strand;
    int a;
    int b;

    int tid;
    uint32_t pos;

    int miss;
    int insert;
    int deletion;

    int geneId;

} map_emt_t2;

typedef struct {
    string name;
    int strand1;
    int tid1;
    int pos1;
    int len1;
    int geneId1;
    vector<uint32_t> cigar1;

    int strand2;
    int tid2;
    int pos2;
    int len2;
    int geneId2;
    vector<uint32_t> cigar2;

    vector<char> seq1;
    vector<char> seq2;

    int type;
    int insert;
    int std;

} encompass_dna_t;

typedef struct {
    int isLeftFirst;

    string name;
    int strand1;
    int tid1;
    uint32_t pos1;
    int len1;

    int strand2;
    int tid2;
    uint32_t pos2;
    int len2;

    int geneId1;
    int geneId2;

    vector<char> seq;

    vector<int> rgIds;

} split_dna_t;

struct result_t {
    string nm5p;
    string nm3p;
    int geneId1 = 0;
    int geneId2 = 0;
    int isCanonical = 0;
    int isReci = 0;
    int numOfEnRna = 0;
    int numOfSpRna = 0;
    int numOfEnDnaT = 0;
    int numOfSpDnaT = 0;
    int numOfEnDnaN = 0;
    int numOfSpDnaN = 0;
    int tier = 0;

    int numOfClusters = 0;
    vector<int> types;
    vector<int> primeOKs;
    vector<int> canos;
    vector<int> numOfsps;

    vector<encompass_rna_t> enrnas;
    vector<split_rna_t> sprnas;

    vector<encompass_dna_t> endna1;
    vector<split_dna_t> spdna1;

    vector<encompass_dna_t> endna2;
    vector<split_dna_t> spdna2;

    int realPrint = 0;

};

typedef struct {
    int min_seq_bwt;
    const char *directoryBWT;

    int cfn;
    double rt;
    int minIntra;
    double minW;

    const char *fileRead;
    const char *fileSum;
    const char *fileEx;
    const char *bkFile;
    const char *bkFileBEDPE;
    const char *bkFileVCF;

    const char *fileJunction;
    const char *filePeptide;
    const char *fileSmcRna;
    const char *fileVirus;
    unordered_map<string, string> virusTypes;

    int isRunningNormal;

    int bacc;
    int largeNum;

    const char *sample_name;

    int minDel;

} options_t;

typedef struct {
    string nm5p;
    string nm3p;

    int tid1;
    int tid2;

    int rnabk1;
    int rnabk2;

    int seqLeft1;
    int seqLeft2;

    int dnabk1;
    int dnabk2;

    split_rna_t splitrna;
    split_dna_t splitdna;

    int swp;

    int rna_only;
    int isExon;

    int exonbk1;
    int exonbk2;

    int gStrand1;
    int gStrand2;

    int tier;
    int isRT;

} break_point_record_t;

// Dec 7, 2015 for peptides
typedef struct {
    int gId;
    int is5p;
    int isCoding;
    int coding_start; // the first full codon starts at position from pos1 or pos2 // has meaning if is5p==true
    int coding_left;  // 0 1 or 2, base left for the last codon of 5p or 0 1 2 for 3p, the first not full codon
    int tid;
    int strand;
    int pos1;
    int pos2;
    string name;
    int exonNum;

} junction_t;
