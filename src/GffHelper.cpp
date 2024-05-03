#include <cstring>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include "Util.h"
#include "GffHelper.h"

using namespace std;

vector<FaSequenceIndex> parseFasta(const char *start, const char *end) {
    // Start pointer of the current line (inclusive)
    const char *pls = start;
    // End pointer of the current line (inclusive)
    const char *ple = pls;
    vector<FaSequenceIndex> seqIndices;
    int currOffset = -1;
    string currSeqName;
    int currLineBasesLength = 0;
    int currLineLength = 0;
    int newlineSize = 1;
    int seqLen = 0;

    while (pls <= end) {
        ple = pls + strcspn(pls, "\n") - 1;
        if (*pls != '>' && (ple - pls) > 0 && currLineLength < 0) {
            currLineLength = ple - pls + 2;
            currLineBasesLength = currLineLength - 1;
            if (*ple == '\r') {
                currLineBasesLength--;
            }
        }
        if (*ple == '\r') {
            ple--;
            newlineSize++;
        }
        if (pls < ple) {
            if (*pls == '>') {
                if (seqIndices.size() > 0) {
                    seqIndices[seqIndices.size() - 1].seqLength = seqLen;
                    seqIndices[seqIndices.size() - 1].lineBasesLength = currLineBasesLength;
                    seqIndices[seqIndices.size() - 1].lineLength = currLineLength;
                }
                currSeqName = string(pls + 1, ple - pls);
                currOffset = ple - start + newlineSize + 1;
                seqIndices.push_back(FaSequenceIndex(currSeqName, -1, currOffset, -1, -1, newlineSize));
                currLineLength = -1;
                seqLen = 0;
            } else {
                seqLen += ple - pls + 1;
            }
        }
        int offset = strcspn(ple, "\n");
        if (offset == 0) {
            break;
        }
        pls = ple + offset + 1;
    }
    if (currOffset >= 0) {
        seqIndices[seqIndices.size() - 1].seqLength = seqLen;
        seqIndices[seqIndices.size() - 1].lineBasesLength = currLineBasesLength;
        seqIndices[seqIndices.size() - 1].lineLength = currLineLength;
    }

    return seqIndices;
}

GffFile parseGff(const char *start, const char *end, const string &fileName) {
    // Start pointer of the current line (inclusive)
    const char *pls = start;
    // End pointer of the current line (inclusive)
    const char *ple = pls;
    string gffVersion;
    vector<GffSequenceRegion> seqRegions;
    vector<GffSequenceFeature> seqFeatures;
    bool inFasta = false;
    int faOffset = -1;
    const char *pfas = nullptr, *pfae = nullptr;
    vector<FaSequenceIndex> faIndices;

    while (pls <= end)
    {
        ple = pls + strcspn(pls, "\n") - 1;
        if (ple <= pls) {
            pls++;
            ple = pls;
            continue;
        }
        int newlineSize = 1;
        if (*ple == '\r') {
            ple--;
            newlineSize++;
        }
        while (pls <= ple && isspace(*pls))
        {
            pls++;
        }
        while (pls <= ple && isspace(*ple))
        {
            ple--;
        }
        if (pls < ple) {
            if (!(*pls == '#' && *(pls + 1) != '#')) {
                int lineLen = ple - pls + 1;
                char *line = (char *)malloc(sizeof(char) * (lineLen + 1));
                copy(pls, ple + 1, line);
                line[lineLen] = '\0';
                string lineStr = string(line);
                free(line);
                if (*pls == '#') {
                    lineStr = lineStr.substr(2);
                    if (lineStr.starts_with("FASTA")) {
                        if (!inFasta) {
                            inFasta = true;
                            pfas = ple + newlineSize + 1;
                            faOffset = ple + newlineSize + 1 - start;
                        }
                    } else {
                        if (lineStr.starts_with("gff-version")) {
                            inFasta = false;
                            lineStr = lineStr.substr(11);
                            gffVersion = trimString(lineStr);
                        } else if (lineStr.starts_with("sequence-region")) {
                            inFasta = false;
                            lineStr = lineStr.substr(15);
                            vector<string> fields = splitString(lineStr, " ");
                            if (fields.size() == 3) {
                                seqRegions.push_back(GffSequenceRegion(fields[0], stoi(fields[1]) - 1, stoi(fields[2])));
                            }
                        }
                    }
                } else {
                    if (!inFasta) {
                        vector<string> fields = splitString(lineStr, "\t");
                        if (fields.size() == 9) {
                            unordered_map<string, string> attribute;
                            vector<string> attributeFields = splitString(fields[8], ";");
                            for (const string &attributeField : attributeFields) {
                                vector<string> keyValue = splitString(attributeField, "=");
                                if (keyValue.size() == 2) {
                                    attribute[trimString(keyValue[0])] = trimString(keyValue[1]);
                                }
                            }
                            GffSequenceFeature seqFeature(fields[0], fields[1], fields[2], stoi(fields[3]) - 1, stoi(fields[4]), fields[5], fields[6], fields[7], attribute);
                            seqFeatures.push_back(seqFeature);
                        }
                    } else {
                        if (pfas == nullptr) {
                            pfas = pls;
                        }
                    }
                }
            }
        }
        pls = ple + strcspn(ple, "\n") + 1;
    }
    if (inFasta) {
        pfae = end;
        vector<FaSequenceIndex> faSectionIdx = parseFasta(pfas, pfae);
        faIndices.insert(faIndices.end(), faSectionIdx.begin(), faSectionIdx.end());
    }

    return GffFile(gffVersion, seqRegions, seqFeatures, faOffset, faIndices);
}
