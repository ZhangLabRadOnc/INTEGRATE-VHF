#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct GffSequenceRegion {
    string seqName;
    int start = 0;
    int end = 0;

    GffSequenceRegion(string seqName, int start, int end) {
        this->seqName = seqName;
        this->start = start;
        this->end = end;
    }
};

struct GffSequenceFeature
{
    string seqName;
    string source;
    string feature;
    int start = 0;
    int end = 0;
    string score;
    string strand;
    string frame;
    unordered_map<string, string> attribute;

    GffSequenceFeature(string seqName, string source, string feature, int start, int end, string score, string strand, string frame, unordered_map<string, string> attribute)
    {
        this->seqName = seqName;
        this->source = source;
        this->feature = feature;
        this->start = start;
        this->end = end;
        this->score = score;
        this->strand = strand;
        this->frame = frame;
        this->attribute = attribute;
    }
};

struct FaSequenceIndex
{
    string seqName;
    int seqLength = 0;
    int offset = 0;
    int lineBasesLength = 0;
    int lineLength = 0;
    int newlineSize = 1;

    FaSequenceIndex(string seqName, int seqLength, int offset, int lineBasesLength, int lineLength, int newlineSize = 1)
    {
        this->seqName = seqName;
        this->seqLength = seqLength;
        this->offset = offset;
        this->lineBasesLength = lineBasesLength;
        this->lineLength = lineLength;
        this->newlineSize = newlineSize;
    }
};

struct GffFile {
    string gffVersion;
    vector<GffSequenceRegion> seqRegions;
    vector<GffSequenceFeature> seqFeatures;
    int faOffset;
    vector<FaSequenceIndex> seqIndices;

    GffFile(string gffVersion, vector<GffSequenceRegion> seqRegions, vector<GffSequenceFeature> seqFeatures, int faOffset, vector<FaSequenceIndex> seqIndices) {
        this->gffVersion = gffVersion;
        this->seqRegions = seqRegions;
        this->seqFeatures = seqFeatures;
        this->faOffset = faOffset;
        this->seqIndices = seqIndices;
    }
};

vector<FaSequenceIndex> parseFasta(const char *start, const char *end);
GffFile parseGff(const char *start, const char *end, const string &fileName);
