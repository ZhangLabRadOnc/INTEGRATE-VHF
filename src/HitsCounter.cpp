/*
 * HitsCounter.cpp
 *
 *  Created on: Jun 14, 2013
 *      Author: jinzhang
 */

#include <filesystem>
#include "BWT.h"
#include "Reference.h"
#include "Util.h"
#include "HitsCounter.h"

using namespace std;
namespace fs = std::filesystem;

HitsCounter::HitsCounter() {
    bwts = nullptr;
    rbwts = nullptr;
}

int HitsCounter::allocate(int size) {
    bwts = new BWT[size];
    rbwts = new BWT[size];
    return 0;
}

int HitsCounter::getChromBWTs(Reference &ref, const char *directory) {
    fs::path bwtPath = fs::path(directory);
    if (!fs::exists(bwtPath)) {
        cerr << directory << " does not exist. Please mkdir " << directory << endl;
        exit(EXIT_FAILURE);
    }

    int size = ref.getSeqCount();

    for (int i = 0; i < size; i++) {
        uint32_t length = ref.getSeqLength(i);

        if (length > MIN_BWT_LEN) {
            cout << "Building BWT and rBWT for " << ref.getSeqOriginalName(i) << "..." << endl;
            float t = clock();

            const char *tmp = ref.getSeq(i);
            getOne(tmp, length, bwtPath / (ref.getSeqOriginalName(i) + ".bwt"));

            char *rtmp = new char[length + 2];

            uint32_t x = 0;
            for (int j = length - 1; j >= 0; j--) {
                rtmp[x++] = getCharComp(tmp[j]);
            }

            rtmp[length] = '$';
            rtmp[length + 1] = '\0';

            getOne(rtmp, length, bwtPath / (ref.getSeqOriginalName(i) + ".rbwt"));
            delete[] rtmp;

            cout << (clock() - t) / CLOCKS_PER_SEC << " seconds\n" << endl;
        }
    }

    return 0;
}

int HitsCounter::getOne(const char *seqRef, uint32_t length, const fs::path &filePath) {
    SuffixArray2 sfa;
    BWT bwt;

    sfa.builtArray(seqRef, length + 1);
    bwt.create(seqRef, length + 1, &sfa);
    bwt.getOccAndOB(seqRef, length + 1);
    bwt.writeTofile(filePath.c_str());
    return 0;
}

int HitsCounter::loadChromBWTs(Reference &ref, const char *directory) {
    fs::path bwtPath = fs::path(directory);
    if (!fs::exists(bwtPath)) {
        cerr << directory << " does not exist. Please mkdir " << directory << endl;
        exit(EXIT_FAILURE);
    }

    int size = ref.getSeqCount();

    int sizeBWTs = 0;

    for (int i = 0; i < size; i++) {
        if (ref.getSeqLength(i) > MIN_BWT_LEN) {
            sizeBWTs++;
        }
    }

    allocate(sizeBWTs);
    number = sizeBWTs;

    int id = 0;

    for (int i = 0; i < size; i++) {
        if (ref.getSeqLength(i) > MIN_BWT_LEN) {
            loadOne(&bwts[id], bwtPath / (ref.getSeqOriginalName(i) + ".bwt"));
            loadOne(&rbwts[id], bwtPath / (ref.getSeqOriginalName(i) + ".rbwt"));

            id++;
        }
    }

    return 0;
}

int HitsCounter::loadOne(BWT *bwt, const fs::path &filePath) {
    FILE *pFile;
    pFile = fopen(filePath.c_str(), "r");

    if (pFile == nullptr) {
        cout << "fail to open " << filePath << endl;
        exit(0);
    } else {
        char tmp[1024];
        fgets(tmp, 1024, pFile);
        tmp[strlen(tmp) - 1] = '\0';
        uint32_t length = atoi(tmp);

        bwt->setLength(length);

        char *seq = new char[length + 1];
        fgets(seq, length + 1, pFile);
        seq[length] = '\0';
        bwt->setBwtSeq(seq);

        fgets(tmp, 1024, pFile);
        fgets(tmp, 1024, pFile);
        tmp[strlen(tmp) - 1] = '\0';
        int distance = atoi(tmp);
        bwt->setDistance(distance);

        int *Occ = new int[256];

        fgets(tmp, 1024, pFile);
        char *cu = tmp;
        char *ne;
        char tmptmp[6];
        tmptmp[0] = '$';
        tmptmp[1] = 'A';
        tmptmp[2] = 'C';
        tmptmp[3] = 'G';
        tmptmp[4] = 'N';
        tmptmp[5] = 'T';

        for (int i = 0; i <= 4; i++) {
            ne = strchr(cu, ' ');
            ne[0] = '\0';
            Occ[tmptmp[i]] = atoi(cu);
            cu = ne + 1;
        }
        Occ['T'] = atoi(cu);

        bwt->setOcc(Occ);

        int saLen = length / distance;
        if (length % distance != 0)
            saLen++;
        char *buffer = new char[(length / distance + 1) * 6 * 10];
        fgets(buffer, (length / distance + 1) * 6 * 10, pFile);
        cu = buffer;
        int *ob = new int[6 * saLen];
        for (int i = 0; i < saLen - 1; i++) {
            for (int j = 0; j < 6; j++) {
                ne = strchr(cu, ' ');
                ne[0] = '\0';
                ob[6 * i + j] = atoi(cu);
                cu = ne + 1; // tmptmp
            }
        }
        for (int j = 0; j < 5; j++) {
            ne = strchr(cu, ' ');
            ne[0] = '\0';
            ob[6 * (saLen - 1) + j] = atoi(cu);
            cu = ne + 1; // tmptmp
        }
        ob[6 * saLen - 1] = atoi(cu);
        bwt->setOBs(ob);

        int *sa = new int[saLen];
        fgets(buffer, length / distance * 6 * 10, pFile);
        cu = buffer;
        for (int i = 0; i < saLen - 1; i++) {
            ne = strchr(cu, ' ');
            ne[0] = '\0';
            sa[i] = atoi(cu);
            cu = ne + 1;
        }
        sa[saLen - 1] = atoi(cu);

        bwt->setSa(sa);

        delete[] buffer;
        fclose(pFile);
    }
    return 0;
}

int HitsCounter::getHitsCount(const char *seq, int len) {

    char rseq[len + 1];
    int x = 0;
    for (int i = len - 1; i >= 0; i--) {
        rseq[x++] = getCharComp(seq[i]);
    }
    rseq[len] = '\0';

    int k, l, mapped;
    // cout<<"AAAA"<<endl;
    int count1 = 0;
    // int mlen1 = 0;
    for (int i = 0; i < number; i++) {
        // cout<<"i="<<i<<endl;
        if (bwts[i].exactSplitMap(k, l, seq, len, mapped, 25) == 1) {
            count1 += l - k + 1;
        }
        // cout<<"outoutout"<<count1<<" "<<mlen1<<endl;
    }
    // cout<<"PPPPP"<<count1<<" "<<mlen1<<endl;
    // cout<<"AAAA2"<<endl;
    int count12 = 0;
    // int mlen12 = 0;
    for (int i = 0; i < number; i++) {
        // cout<<"i="<<i<<endl;
        if (rbwts[i].exactSplitMap(k, l, seq, len, mapped, 25) == 1) {
            count12 += l - k + 1;
        }
    }
    // cout<<"PPPPP"<<count12<<" "<<mlen12<<endl;

    int count2 = 0;
    // int mlen2 = 0;

    // cout<<"BBBB"<<endl;
    for (int i = 0; i < number; i++) {
        // cout<<"i="<<i<<endl;
        int k, l, mapped;
        if (bwts[i].exactSplitMap(k, l, rseq, len, mapped, 25) == 1) {
            count2 += l - k + 1;
        }
    }
    int count22 = 0;
    // int mlen22 = 0;
    // cout<<"PPPPP"<<count2<<" "<<mlen2<<endl;
    // cout<<"BBBB2"<<endl;
    for (int i = 0; i < number; i++) {
        // cout<<"i="<<i<<endl;
        if (rbwts[i].exactSplitMap(k, l, rseq, len, mapped, 25) == 1) {
            count22 += l - k + 1;
        }
    }
    // cout<<"PPPPP"<<count22<<" "<<mlen22<<endl;
    int max = ((count1 + count12) > (count2 + count22)) ? (count1 + count12) : (count2 + count22);
    // cout<<"PPPPPMMMMM"<<mincount<<endl;
    return max;
}

HitsCounter::~HitsCounter() {
    // TODO Auto-generated destructor stub
    if (bwts != nullptr)
        delete[] bwts;
    if (rbwts != nullptr)
        delete[] rbwts;
}
