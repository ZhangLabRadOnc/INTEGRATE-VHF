/*
 * MyBamHeader.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: jinzhang
 */

#include "MyBamHeader.h"
#include <unordered_map>

int MAX_LIB_INSERT = 10000;

MyBamHeader::MyBamHeader() {
    f = nullptr;
    h = nullptr;
    maxDistance = 0;
    mInsert = 0;
    mStd = 0;
    numTids = 0;
    isRG = 0; // no RG
}

MyBamHeader::~MyBamHeader()
{
    if (h != nullptr)
    {
        sam_hdr_destroy(h);
        h = nullptr;
    }
    if (f != nullptr) {
        sam_close(f);
        f = nullptr;
    }
}

int MyBamHeader::myBamOpen(const char *fileName) {
    f = sam_open(fileName, "r");
    if (f == nullptr)
    {
        cerr << "Failed to open file: " << fileName << endl;
        exit(1);
    }
    h = sam_hdr_read(f);
    if (h == nullptr)
    {
        sam_close(f);
        cerr << "Failed to get header of file: " << fileName << endl;
        exit(1);
    }

    return 0;
}

int MyBamHeader::getRGs() {

    char *line_buffer = h->text;
    // cout<<strlen(line_buffer)<<endl;
    int length = h->l_text;
    // cout<<length<<"l"<<endl;

    char *p = nullptr;
    char *p2 = nullptr;
    char *nextN = nullptr;

    char id[1024];
    char insert[1024];
    id[0] = '\0';
    insert[0] = '\0';

    while (true) {
        nextN = strchr(line_buffer, '\n');
        if (line_buffer[0] == '@' && line_buffer[1] == 'R' && line_buffer[2] == 'G') {
            char tmp[nextN - line_buffer + 2];
            tmp[nextN - line_buffer + 1] = '\0';
            memcpy(tmp, line_buffer, nextN - line_buffer + 1);

            p = strstr(tmp, "ID:");
            p2 = nullptr;
            for (int i = 0; i < strlen(p); i++) {
                if (p[i] == ' ' || p[i] == '\t' || p[i] == '\n') {
                    p2 = p + i;
                    break;
                }
            }
            if (p2 == nullptr) {
                cerr << "fail to pass header." << endl;
            } else {
                memcpy(id, p + 3, p2 - p - 3);
                id[p2 - p - 3] = '\0';
            }

            p = strstr(tmp, "PI:");
            if (p != nullptr) {
                p2 = nullptr;
                for (int i = 0; i < strlen(p); i++) {
                    if (p[i] == ' ' || p[i] == '\t' || p[i] == '\n') {
                        p2 = p + i;
                        break;
                    }
                }
                if (p2 == nullptr) {
                    cerr << "fail to pass header." << endl;
                } else {
                    memcpy(insert, p + 3, p2 - p - 3);
                    insert[p2 - p - 3] = '\0';
                }

                rg.insert(pair<string, int>(string(id), atoi(insert)));
            } else {
                rg.insert(pair<string, int>(string(id), 0));
            }
        }
        if (nextN - h->text == length - 1) {
            break;
        } else {
            line_buffer = nextN + 1;
        }
    }
    /*
            if(rg.size()>0)
            {
                    cout<<"RGs are: "<<endl;
                    for (map<string,int>::iterator it=rg.begin(); it!=rg.end(); ++it)
                            cout <<"ID:"<<it->first<<"\tPI:"<<it->second<<'\n';
                    isRG=1;
            }
            else
            {
                    cout<<"no RG lines found"<<endl;
            }

            if(isRG==1)
            {
                    getRGStd(std);
                    for (map<string,int>::iterator it=rg.begin(); it!=rg.end(); ++it)
                    {
                            if(it->second==0)
                            {
                                    it->second=mInsert;
                                    cout<<"Got 0, changed to default"<<endl;
                            }
                    }
            }
    */

    return 0;
}

int getMeanStd(vector<int> numbers, int &mean, int &std) {
    double sum = 0.0;
    double sumSquare = 0.0;
    for (int i = 0; i < numbers.size(); i++) {
        sum += numbers[i];
    }
    mean = sum / numbers.size();
    for (int i = 0; i < numbers.size(); i++) {
        sumSquare += (double)(numbers[i] - mean) * (double)(numbers[i] - mean) / (double)numbers.size();
    }

    std = sqrt(sumSquare);

    return 0;
}

//PREVIOUS Approach
// bool testInsertStd(const char *fileName, string testRG, int &insert, int &std) {
//     vector<int> inserts;

//     samFile *f = sam_open(fileName, "r");
//     bam_hdr_t *h = sam_hdr_read(f);
//     bam1_t *b = bam_init1();

//     int x = 0;
//     while (true)
//     {
//         if ((sam_read1(f, h, b)) < 0)
//         {
//             break;
//         }
//         else
//         {
//             if (x >= 10000000)
//             {
//                 break;
//             }
//             x++;
//             char readg[1024];
//             readg[0] = '\0';
//             strcat(readg, bam_aux2Z(bam_aux_get(b, "RG")));
//             string tmp(readg);
//             if (testRG.compare(tmp) == 0)
//             {
//                 if (b->core.tid == b->core.mtid && b->core.pos < b->core.mpos && b->core.mpos - b->core.pos < MAX_LIB_INSERT)
//                 {
//                     inserts.push_back(b->core.mpos - b->core.pos + b->core.l_qseq);
//                 }
//             }
//         }
//     }

//     getMeanStd(inserts, insert, std);

//     bam_destroy1(b);
//     sam_hdr_destroy(h);
//     sam_close(f);
    
//     return inserts.size() > 0;
// }

//Old TestInsertStdAll
// bool testInsertStdAll(const char *fileName, int &insert, int &std) {
//     vector<int> inserts;

//     samFile *f = sam_open(fileName, "r");
//     sam_hdr_t *h = sam_hdr_read(f);
//     bam1_t *b = bam_init1();

//     int x = 0;
//     while (true)
//     {
//         if ((sam_read1(f, h, b)) < 0)
//         {
//             break;
//         }
//         else
//         {
//             if (x >= 10000000)
//             {
//                 break;
//             }
//             x++;

//             if (b->core.tid == b->core.mtid && b->core.pos < b->core.mpos && b->core.mpos - b->core.pos < MAX_LIB_INSERT)
//             {
//                 inserts.push_back(b->core.mpos - b->core.pos + b->core.l_qseq);
//             }
//         }
//     }

//     getMeanStd(inserts, insert, std);

//     bam_destroy1(b);
//     sam_hdr_destroy(h);
//     sam_close(f);

//     return inserts.size() > 0;
// }
struct PairInfo {
    int32_t isize;
    bool fwd;
    bool first;
    bool seen = false;
};

bool testInsertStd(const char *fileName, const std::string &testRG, int &insertMean, int &insertStd) {

    samFile *fp = sam_open(fileName, "r");
    if (!fp) return false;

    bam_hdr_t *h = sam_hdr_read(fp);
    bam1_t *b = bam_init1();

    std::unordered_map<std::string, PairInfo> pairs;
    std::vector<int> allInward;  // store all inward pairs first

    while (sam_read1(fp, h, b) >= 0) {
        uint16_t flag = b->core.flag;

        if (!(flag & BAM_FPAIRED)) continue;
        if (flag & (BAM_FUNMAP | BAM_FMUNMAP | BAM_FSECONDARY |
                     BAM_FSUPPLEMENTARY | BAM_FQCFAIL | BAM_FDUP)) continue;
        if (b->core.tid != b->core.mtid) continue;

        uint8_t *rg = bam_aux_get(b, "RG");
        if (!rg) continue;
        const char *rgVal = bam_aux2Z(rg);
        if (!rgVal) continue;
        if (testRG != rgVal) continue;

        int32_t isize = b->core.isize;
        if (isize == 0) continue;
        if (isize < 0) isize = -isize;
        if (isize > MAX_LIB_INSERT) continue;

        bool fwd = !(flag & BAM_FREVERSE);
        bool first = flag & BAM_FREAD1;

        std::string qname = bam_get_qname(b);
        auto it = pairs.find(qname);

        if (it == pairs.end()) {
            pairs[qname] = {isize, fwd, first, false};
        } else {
            PairInfo &mate = it->second;

            if (!mate.seen) {
                // simplified inward check
                bool inward = false;
                if ((mate.first && mate.fwd && !fwd) || (mate.first && !mate.fwd && fwd) ||
                    (!mate.first && !mate.fwd && fwd) || (!mate.first && mate.fwd && !fwd)) {
                    inward = true;
                }

                if (inward) allInward.push_back(isize);
                mate.seen = true;
            }

            pairs.erase(it);
        }
    }

    bam_destroy1(b);
    bam_hdr_destroy(h);
    sam_close(fp);

    if (allInward.empty()) return false;

    // Build histogram of insert sizes
    std::map<int, int> hist;  // sorted by insert size
    for (int x : allInward) hist[x]++;

    // Pick main bulk (~99% of total pairs)
    int totalPairs = allInward.size();
    int cum = 0;
    int bulkLimit = static_cast<int>(totalPairs * 0.99);

    double sum = 0.0;
    double sumSq = 0.0;
    int bulkCount = 0;

    for (auto &[isize, count] : hist) {
        if (cum + count > bulkLimit) count = bulkLimit - cum;  // only include remaining to reach 99%
        for (int i = 0; i < count; i++) {
            sum += isize;
            sumSq += isize * isize;
        }
        bulkCount += count;
        cum += count;
        if (cum >= bulkLimit) break;
    }

    if (bulkCount == 0) return false;

    double mean = sum / bulkCount;
    double sd = std::sqrt(sumSq / bulkCount - mean * mean);

    insertMean = static_cast<int>(mean);
    insertStd = static_cast<int>(sd);
    return true;
}

bool testInsertStdAll(const char *fileName, int &insertMean, int &insertStd) {

    samFile *fp = sam_open(fileName, "r");
    if (!fp) return false;

    bam_hdr_t *h = sam_hdr_read(fp);
    bam1_t *b = bam_init1();

    std::unordered_map<std::string, PairInfo> pairs;
    std::vector<int> allInward;

    while (sam_read1(fp, h, b) >= 0) {
        uint16_t flag = b->core.flag;

        if (!(flag & BAM_FPAIRED)) continue;
        if (flag & (BAM_FUNMAP | BAM_FMUNMAP | BAM_FSECONDARY |
                     BAM_FSUPPLEMENTARY | BAM_FQCFAIL | BAM_FDUP)) continue;
        if (b->core.tid != b->core.mtid) continue;

        int32_t isize = b->core.isize;
        if (isize == 0) continue;
        if (isize < 0) isize = -isize;
        if (isize > MAX_LIB_INSERT) continue;

        bool fwd = !(flag & BAM_FREVERSE);
        bool first = flag & BAM_FREAD1;

        std::string qname = bam_get_qname(b);
        auto it = pairs.find(qname);

        if (it == pairs.end()) {
            pairs[qname] = {isize, fwd, first, false};
        } else {
            PairInfo &mate = it->second;

            if (!mate.seen) {
                bool inward = false;
                if ((mate.first && mate.fwd && !fwd) || (mate.first && !mate.fwd && fwd) ||
                    (!mate.first && !mate.fwd && fwd) || (!mate.first && mate.fwd && !fwd)) {
                    inward = true;
                }

                if (inward) allInward.push_back(isize);
                mate.seen = true;
            }

            pairs.erase(it);
        }
    }

    bam_destroy1(b);
    bam_hdr_destroy(h);
    sam_close(fp);

    if (allInward.empty()) return false;

    // Build histogram of insert sizes
    std::map<int, int> hist;
    for (int x : allInward) hist[x]++;

    // Pick main bulk (~99%)
    int totalPairs = allInward.size();
    int cum = 0;
    int bulkLimit = static_cast<int>(totalPairs * 0.99);

    double sum = 0.0;
    double sumSq = 0.0;
    int bulkCount = 0;

    for (auto &[isize, count] : hist) {
        if (cum + count > bulkLimit) count = bulkLimit - cum;
        for (int i = 0; i < count; i++) {
            sum += isize;
            sumSq += isize * isize;
        }
        bulkCount += count;
        cum += count;
        if (cum >= bulkLimit) break;
    }

    if (bulkCount == 0) return false;

    double mean = sum / bulkCount;
    double sd = std::sqrt(sumSq / bulkCount - mean * mean);

    insertMean = static_cast<int>(mean);
    insertStd = static_cast<int>(sd);

    return true;
}

int MyBamHeader::getInsertStdFromBAM(const char *filename) {
    if (rg.size() > 0) {
        isRG = 1;
        for (map<string, int>::iterator it = rg.begin(); it != rg.end(); ++it) {
            int insert, std;
            if (testInsertStd(filename, it->first, insert, std)) {
                if (it->second == 0 ||it->second - insert > std || it->second - insert < 0 - std) {
                    cout << "Warning: for RG " << it->first << ", there is no PI in BAM or the value provided is " << it->second << " and tested is " << insert
                         << ". Changed to tested value." << endl;
                    // cout << "[DEBUG3] : it->second = "<< it->second<< "insert = "<< insert<<  ", std = " << std << endl;
                    it->second = insert;
                    this->std.insert(pair<string, int>(string(it->first), std));
                } else {
                    this->std.insert(pair<string, int>(string(it->first), std));
                }
            } else {
                cout << "Warning: for RG " << it->first << ", testing failed. Set values to " << mInsert << " " << mStd << endl;
            }
        }
    }

    return 0;
}

int MyBamHeader::getRGStd(int se) {
    // for now. just make things easy. later please refine and make it right
    for (map<string, int>::iterator it = rg.begin(); it != rg.end(); ++it) {
        std.insert(pair<string, int>(string(it->first), se));
    }
    return 0;
}

int MyBamHeader::getPI(char *rgp) { return rg[string(rgp)]; }

int MyBamHeader::getStd(char *rgp) { return std[string(rgp)]; }

string MyBamHeader::getChrName(int tid) {
    return string(h->target_name[tid]);
}

/*
int MyBamHeader::run(const char * fileName) {

        this->setMInsert(440);//this is lazy need parameter pass
        this->setMStd(52);

        myBamOpen(fileName);
        getRGs(50);

        maxDistance=computeMax();

        //cout<<"maxDis="<<maxDistance<<endl;

        setTidM();

        //and here need another function to test PI and stds, in case they are still not set

        return 0;
}
*/

int MyBamHeader::printRGs() {

    if (isRG == 1) {
        cout << "Insert sizes and stds tested by different RGs: " << endl;
        for (map<string, int>::iterator it = rg.begin(); it != rg.end(); ++it) {
            cout << it->first << "\t" << it->second << " " << this->std[it->first] << endl;
        }
        return 0;
    } else {
        cout << "Insert sizes and stds according to testing are:" << endl;
        cout << mInsert << " " << mStd << endl;
    }

    return 0;
}

int MyBamHeader::run2(const char *fileName) {

    // this->setMInsert(insert);
    // this->setMStd(std);

    int insertt, stdt;
    if (testInsertStdAll(fileName, insertt, stdt)) {
        mInsert = insertt;
        mStd = stdt;
    }

    myBamOpen(fileName);
    getRGs();

    getInsertStdFromBAM(fileName);

    printRGs();

    maxDistance = computeMax();

    // cout<<"maxDis="<<maxDistance<<endl;
    numTids = h->n_targets;

    // and here need another function to test PI and stds, in case they are still not set

    return 0;
}

int MyBamHeader::computeMax() {
    int max = 0;
    map<string, int>::iterator it2 = std.begin();
    for (map<string, int>::iterator it = rg.begin(); it != rg.end(); ++it) {
        int ii = it->second;
        int ss = it2->second;
        // cout<<ii<<" "<<ss<<" "<<ii+3*ss<<endl;
        if (ii + 3 * ss > max) {
            max = ii + 3 * ss;
        }
        ++it2;
    }
    // cout<<"max="<<max<<endl;
    if (max == 0)
        max = mInsert + 3 * mStd;
    // cout<<"max="<<max<<endl;
    return max;
}
