/*
 * Gene.cpp
 *
 *  Created on: Apr 28, 2013
 *      Author: jinzhang
 */

#include "GffHelper.h"
#include "Util.h"
#include "VirusLoader.h"
#include "Gene.h"

Gene::Gene() {
    this->bwts = nullptr;
    this->rbwts = nullptr;
}

Gene::~Gene()
{
    if (this->bwts != nullptr)
    {
        delete[] this->bwts;
    }
    if (this->rbwts != nullptr)
    {
        delete[] this->rbwts;
    }
    for (int i = 0; i < this->transcripts.size(); i++)
    {
        if (this->transcripts[i].exonStarts != nullptr)
        {
            delete[] this->transcripts[i].exonStarts;
        }
        if (this->transcripts[i].exonEnds != nullptr)
        {
            delete[] this->transcripts[i].exonEnds;
        }
    }
    this->transcripts.clear();
    this->viruses.clear();
}

bool myGeneSortFunc(gene_t i, gene_t j) {
    if (i.tid < j.tid) {
        return true;
    } else if (i.tid == j.tid) {
        if (i.leftLimit < j.leftLimit)
            return true;
        else
            return false;
    } else
        return false;
}

bool myTransSortFunc(transcript_t i, transcript_t j) {
    if (i.name2.compare(j.name2) < 0)
        return true;
    else if (i.name2.compare(j.name2) == 0) {
        if (i.tid < j.tid) {
            return true;
        } else if (i.tid == j.tid) {
            if (i.txStart < j.txStart)
                return true;
            else
                return false;
        } else
            return false;

    } else
        return false;
}

int Gene::loadGenesFromFile(const char *fileName, const vector<VirusLoader::VirusNamePair> *namePairs, Reference &ref) {
    int fd;
    size_t fileLen;
    int count = 0;
    // Start pointer of the file (inclusive)
    const char *pfs = openFileForRead(fileName, fd, fileLen);
    // End pointer of the file (exclusive)
    const char *pfe = pfs + fileLen - 1;
    // Start pointer of the current line (inclusive)
    const char *pls = pfs;
    // End pointer of the current line (inclusive)
    const char *ple = pls;
    while (pls <= pfe)
    {
        ple = pls + strcspn(pls, "\n") - 1;
        if (ple <= pls) {
            pls++;
            ple = pls;
            continue;
        }
        if (*ple == '\r') {
            ple--;
        }
        while (pls <= ple && isspace(*pls))
        {
            pls++;
        }
        while (pls <= ple && isspace(*ple))
        {
            ple--;
        }
        if (!(pls >= ple || *pls == '#'))
        {
            int lineLen = ple - pls + 1;
            char *line = (char *)malloc(sizeof(char) * (lineLen + 1));
            copy(pls, ple + 1, line);
            line[lineLen] = '\0';

            transcript_t tt;
            const char *delimiters = " \t";
            char *token = strtok(line, delimiters);
            tt.name = string(token);
            token = strtok(nullptr, delimiters);
            tt.chrName = string(token);
            tt.tid = -1;
            if (namePairs != nullptr)
            {
                for (const auto &pair : *namePairs)
                {
                    if (pair.originalName.compare(tt.chrName) == 0)
                    {
                        tt.chrName = pair.mappedName;
                        break;
                    }
                }
            } else {
                tt.chrName = getStdChrName(tt.chrName);
            }
            tt.tid = ref.getSeqIdByMappedName(tt.chrName);
            if (tt.tid < 0)
            {
                free(line);
                pls = ple + strcspn(ple, "\n") + 1;
                continue;
            }
            token = strtok(nullptr, delimiters);
            tt.strand = (token[0] == '+' ? 0 : 1);
            token = strtok(nullptr, delimiters);
            tt.txStart = stoi(token);
            token = strtok(nullptr, delimiters);
            tt.txEnd = stoi(token);
            token = strtok(nullptr, delimiters);
            tt.cdsStart = stoi(token);
            token = strtok(nullptr, delimiters);
            tt.cdsEnd = stoi(token);
            token = strtok(nullptr, delimiters);
            tt.exonCount = stoi(token);
            token = strtok(nullptr, delimiters);
            char *exonStartsC = strdup(token);
            token = strtok(nullptr, delimiters);
            char *exonEndsC = strdup(token);
            token = strtok(nullptr, delimiters);
            tt.name2 = string(token);
            free(line);

            uint32_t *pes = new uint32_t[tt.exonCount];
            uint32_t *pee = new uint32_t[tt.exonCount];

            int32_t idx = 0;
            int32_t num = 0;
            for (const char* p = exonStartsC; *p && idx < tt.exonCount; ++p) {
                if (*p >= '0' && *p <= '9') {
                    num = num * 10 + (*p - '0');
                } else if (*p == ',') {
                    pes[idx++] = num;
                    num = 0;
                } else {
                    cerr << "Error parsing gene annotation." << endl;
                    exit(EXIT_FAILURE);
                }
            }
            free(exonStartsC);

            idx = 0;
            num = 0;
            for (const char* p = exonEndsC; *p && idx < tt.exonCount; ++p) {
                if (*p >= '0' && *p <= '9') {
                    num = num * 10 + (*p - '0');
                } else if (*p == ',') {
                    pee[idx++] = num;
                    num = 0;
                } else {
                    cerr << "Error parsing gene annotation." << endl;
                    exit(EXIT_FAILURE);
                }
            }
            free(exonEndsC);

            tt.exonStarts = pes;
            tt.exonEnds = pee;

            // Dec 2015, let us not use the transcripts truncated in coding region
            if (!(tt.cdsStart != tt.cdsEnd && ((tt.txStart == tt.cdsStart) || (tt.txEnd == tt.cdsEnd))))
            {
                transcripts.push_back(tt);
                count++;
            }
        }
        pls = ple + strcspn(ple, "\n") + 1;
    }
    closeFileForRead(pfs, fd, fileLen);

    cout << count << " transcripts loaded from file: " << fileName << endl;

    return 0;
}

void Gene::readVirusLoaderTSV(const string &k, const vector<VirusLoader::VirusNamePair> &v, Reference &ref)
{
    loadGenesFromFile(k.c_str(), &v, ref);
    for (const auto &p : v)
    {
        viruses.push_back(p.mappedName);
    }
}

bool compareGffSeqFeature(const GffSequenceFeature &a, const GffSequenceFeature &b) {
    return a.start < b.start;
}

void Gene::readVirusLoaderGFF(const string &k, const vector<VirusLoader::VirusNamePair> &v, Reference &ref)
{
    string filePath = k;
    for (const auto &p : v)
    {
        viruses.push_back(p.mappedName);
    }
    int fd;
    size_t fileLen;
    const char *pfs = openFileForRead(filePath, fd, fileLen);
    // End pointer of the file (exclusive)
    const char *pfe = pfs + fileLen - 1;

    GffFile gf = parseGff(pfs, pfe, filePath);
    if (ref.getSeqIdByOriginalName(gf.seqRegions[0].seqName) < 0) {
        ref.readVirusLoaderGff(gf, pfs, v);
    }
    vector<transcript_t> gffTranscripts;

    for (GffSequenceFeature &seqFeature : gf.seqFeatures)
    {
        if (seqFeature.attribute.contains("ID"))
        {
            seqFeature.attribute["ID"] = replaceString(seqFeature.attribute.at("ID"), "%2A", "*");
        }
        if (seqFeature.attribute.contains("Name"))
        {
            seqFeature.attribute["Name"] = replaceString(seqFeature.attribute.at("Name"), "%2A", "*");
        }
        if (seqFeature.attribute.contains("Parent"))
        {
            seqFeature.attribute["Parent"] = replaceString(seqFeature.attribute.at("Parent"), "%2A", "*");
        }
    }
    unordered_map<string, vector<GffSequenceFeature>> cdsFeaturesMap;
    for (int i = 0; i < gf.seqFeatures.size(); i++)
    {
        const GffSequenceFeature &seqFeature = gf.seqFeatures[i];

        if (seqFeature.feature.compare("CDS") == 0)
        {
            string ID = seqFeature.attribute.at("ID");
            if (cdsFeaturesMap.contains(ID))
            {
                cdsFeaturesMap[ID].push_back(seqFeature);
            }
            else
            {
                cdsFeaturesMap[ID] = {seqFeature};
            }
        }
    }
    int idx = 0;
    for (auto it = cdsFeaturesMap.begin(); it != cdsFeaturesMap.end(); ++it)
    {
        vector<GffSequenceFeature> &cdsFeatures = it->second;
        const GffSequenceFeature &cdsFeature = cdsFeatures[0];
        sort(cdsFeatures.begin(), cdsFeatures.end(), compareGffSeqFeature);
        if (cdsFeatures.size() >= 2)
        {
            GffSequenceFeature &featureFirst = cdsFeatures[0];
            if (featureFirst.end - featureFirst.start + 1 < 300)
            {
                featureFirst.start = max(featureFirst.end - 300, 1);
            }
            GffSequenceFeature &featureLast = cdsFeatures[cdsFeatures.size() - 1];
            if (featureLast.end - featureLast.start + 1 < 300)
            {
                featureLast.end = featureLast.start + 300;
            }
        }
        sort(cdsFeatures.begin(), cdsFeatures.end(), compareGffSeqFeature);
        string name2 = "N/A";
        for (int i = 0; i < gf.seqFeatures.size(); i++)
        {
            const GffSequenceFeature &seqFeature = gf.seqFeatures[i];

            if (seqFeature.feature.compare("gene") == 0)
            {
                if (seqFeature.attribute.at("ID").compare(cdsFeature.attribute.at("Parent")) == 0)
                {
                    name2 = seqFeature.attribute.at("Name");
                    break;
                }
            }
        }
        for (int i = 0; i < cdsFeatures.size(); i++, ++idx)
        {
            const GffSequenceFeature &cdsFeature = cdsFeatures[i];
            transcript_t tt;
            tt.name = to_string(idx);
            tt.chrName = cdsFeature.seqName;
            for (const auto &p : v)
            {
                if (p.originalName.compare(cdsFeature.seqName) == 0)
                {
                    tt.chrName = p.mappedName;
                    break;
                }
            }
            tt.tid = ref.getSeqIdByMappedName(tt.chrName);
            tt.strand = cdsFeature.strand.compare("+") == 0 ? 0 : 1;
            tt.txStart = cdsFeature.start;
            tt.txEnd = cdsFeature.end;
            tt.cdsStart = cdsFeature.end;
            tt.cdsEnd = cdsFeature.end;
            tt.exonCount = 1;
            tt.exonStarts = (uint32_t *)malloc(sizeof(uint32_t) * tt.exonCount);
            tt.exonEnds = (uint32_t *)malloc(sizeof(uint32_t) * tt.exonCount);
            tt.exonStarts[0] = cdsFeature.start;
            tt.exonEnds[0] = cdsFeature.end;
            tt.name2 = name2;
            if (!(tt.cdsStart != tt.cdsEnd && ((tt.txStart == tt.cdsStart) || (tt.txEnd == tt.cdsEnd))))
            {
                gffTranscripts.push_back(tt);
            }
        }
    }
    closeFileForRead(pfs, fd, fileLen);
    transcripts.insert(transcripts.end(), gffTranscripts.begin(), gffTranscripts.end());
    cout << gffTranscripts.size() << " transcripts loaded from file: " << filePath << endl;
}

void Gene::sortTranscripts() {
    sort(this->transcripts.begin(), this->transcripts.end(), myTransSortFunc);
}

int Gene::setGene() {
    int fid = 0;
    if (transcripts.size() < 1) {
        cerr << "No gene annotation" << endl;
        exit(1);
    } else {
        gene_t gt;
        gt.name2 = transcripts[0].name2;
        gt.chrName = transcripts[0].chrName;
        gt.strand = transcripts[0].strand;
        gt.transIds.push_back(0);
        gt.leftLimit = transcripts[0].txStart;
        gt.rightLimit = transcripts[0].txEnd;
        gt.tid = transcripts[0].tid;
        gt.fakeId = -1;
        genes.push_back(gt);
    }

    for (int i = 1; i < transcripts.size(); i++) {
        if (transcripts[i].name2.compare(transcripts[i - 1].name2) != 0) {
            gene_t gt;
            gt.name2 = transcripts[i].name2;
            gt.chrName = transcripts[i].chrName;
            gt.strand = transcripts[i].strand;
            gt.transIds.push_back(i);
            gt.leftLimit = transcripts[i].txStart;
            gt.rightLimit = transcripts[i].txEnd;
            gt.tid = transcripts[i].tid;
            gt.fakeId = -1;
            genes.push_back(gt);
        } else {

            if (transcripts[i].tid != transcripts[i - 1].tid) {
                gene_t gt;
                gt.name2 = transcripts[i].name2;
                gt.chrName = transcripts[i].chrName;
                gt.strand = transcripts[i].strand;
                gt.transIds.push_back(i);
                gt.leftLimit = transcripts[i].txStart;
                gt.rightLimit = transcripts[i].txEnd;
                gt.tid = transcripts[i].tid;
                if (genes[genes.size() - 1].fakeId != -1)
                    gt.fakeId = fid++;
                else {
                    genes[genes.size() - 1].fakeId = fid++;
                    gt.fakeId = fid++;
                }
                genes.push_back(gt);
                continue;
            } else {
                if (transcripts[i].txStart > genes[genes.size() - 1].rightLimit) {
                    gene_t gt;
                    gt.name2 = transcripts[i].name2;
                    gt.chrName = transcripts[i].chrName;
                    gt.strand = transcripts[i].strand;
                    gt.transIds.push_back(i);
                    gt.leftLimit = transcripts[i].txStart;
                    gt.rightLimit = transcripts[i].txEnd;
                    gt.tid = transcripts[i].tid;
                    if (genes[genes.size() - 1].fakeId != -1)
                        gt.fakeId = fid++;
                    else {
                        genes[genes.size() - 1].fakeId = fid++;
                        gt.fakeId = fid++;
                    }
                    genes.push_back(gt);
                    continue;
                }
            }

            // if(transcripts[i].txStart<genes[genes.size()-1].leftLimit)//this is not possible now
            //{
            //	genes[genes.size()-1].leftLimit=transcripts[i].txStart;
            //}
            if (transcripts[i].txEnd > genes[genes.size() - 1].rightLimit) {
                genes[genes.size() - 1].rightLimit = transcripts[i].txEnd;
            }
            genes[genes.size() - 1].transIds.push_back(i);
        }
    }

    sort(genes.begin(), genes.end(), myGeneSortFunc);
    //	cout<<genes.size()<<" genes."<<endl;
    //	cout<<fid<<" of them are genes at different chroms or locations."<<endl;

    /*
            for(int i=0;i<genes.size();i++)
            {
                    cout<<i<<" "<<genes[i].fakeId<<" "<<genes[i].name2<<" "<<genes[i].tid<<" "<<genes[i].leftLimit<<" "<<genes[i].rightLimit<<endl;
            }
    */
    return 0;
}

int Gene::isInGene(int tid, uint32_t pos, vector<int> &geneIds) {

    gene_t dumbGene;
    dumbGene.tid = tid;
    dumbGene.leftLimit = pos;
    // cout<<tid<<"\t"<<pos<<"\t";

    vector<gene_t>::iterator up = upper_bound(genes.begin(), genes.end(), dumbGene, myGeneSortFunc);
    // cout<<up-genes.begin()<<endl;
    if (up - genes.begin() == 0)
        return 0;
    up--;
    while (up - genes.begin() >= 0 && (*up).tid == tid && pos > (*up).leftLimit) {
        if ((*up).leftLimit < pos && (*up).rightLimit > pos) {
            geneIds.push_back(up - genes.begin());
        }
        up--;
    }
    if (geneIds.size() > 0) {
        // cout<<"Marked"<<endl;
        return 1;
    } else
        return 0;
}

bool Gene::isPairPossibleFusion(int id1, int id2, int strand1, int strand2) {

    // if(genes[id1].fakeId !=-1 && genes[id1].fakeId==genes[id2].fakeId)
    //	return 0;

    if (genes[id1].name2.compare(genes[id2].name2) == 0) {
        return false;
    }

    bool found = false;

    for (auto &v : viruses) {
        if (v.compare(genes[id1].chrName) == 0 || v.compare(genes[id2].chrName) == 0) {
            found = true;
            break;
        }
    }
    if (!found && viruses.size() > 0)
    {
        return false;
    }
    
    int gStrand1 = genes[id1].strand;
    int gStrand2 = genes[id2].strand;

    int UoD1, UoD2;
    if (gStrand1 + strand1 == 1) {
        UoD1 = 0;
    } else {
        UoD1 = 1;
    }

    if (gStrand2 + strand2 == 1) {
        UoD2 = 0;
    } else {
        UoD2 = 1;
    }

    if (UoD1 + UoD2 != 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

gene_t *Gene::getGene(int index) { return &(genes[index]); }

int Gene::addRnaAnchor(int anId, int geneId) {
    genes[geneId].anchors.push_back(anId);
    return 0;
}

bool mySortExon(exon_map_t i, exon_map_t j) {
    if (i.start < j.start) {
        return true;
    } else if (i.start < j.start) {
        if (i.end < j.end) {
            return true;
        } else
            return false;
    } else
        return false;
}

int Gene::getExons(int geneId, list<exon_map_t> &exons) {

    for (int i = 0; i < genes[geneId].transIds.size(); i++) {
        // cout<<"trans"<<endl;
        int transId = genes[geneId].transIds[i];
        for (int j = 0; j < transcripts[transId].exonCount; j++) {
            // cout<<"exon"<<endl;
            exon_map_t ex;
            ex.tid = genes[geneId].tid;
            ex.start = transcripts[transId].exonStarts[j];
            ex.end = transcripts[transId].exonEnds[j];
            ex.strand = transcripts[transId].strand;
            ex.geneId = geneId;
            ex.transIds.push_back(transId);
            ex.exonIds.push_back(j);
            exons.push_back(ex);
        }
    }

    exons.sort(mySortExon);

    list<exon_map_t>::iterator it = exons.begin();
    while (true) {
        it++;
        if (it == exons.end())
            break;
        list<exon_map_t>::iterator it2 = it;
        it--;

        if ((*it).start == (*it2).start && (*it).end == (*it2).end) {
            for (int k = 0; k < (*it2).transIds.size(); k++) {
                (*it).transIds.push_back((*it2).transIds[k]);
                (*it).exonIds.push_back((*it2).exonIds[k]);
            }
            exons.erase(it2);
        } else
            it++;
    }
    return 0;
}

int Gene::pushAnchor(int geneId, int id) { // should create something and put with the graph not in gene!!!!
    genes[geneId].anchors.push_back(id);
    return 0;
}

/*
int Gene::getPartialSize(int geneId) {
        return genes[geneId].partialMaps.size();
}

int Gene::getPartialData(int geneId, int index) {
        return genes[geneId].partialMaps[index];
}


int Gene::getPartialSizeM(int geneId) {
        return genes[geneId].partialMapsM.size();
}

int Gene::getPartialDataM(int geneId, int index) {
        return genes[geneId].partialMapsM[index];
}

*/
uint32_t Gene::getStartPos(int tranId, int exonId) { return transcripts[tranId].exonStarts[exonId]; }

uint32_t Gene::getEndPos(int tranId, int exonId) { return transcripts[tranId].exonEnds[exonId]; }

int Gene::getTid(int geneId) { return genes[geneId].tid; }

int Gene::buildOneSuffix(int geneId, int isForward, Reference &ref) {
    // cout<<"in one"<<endl;

    int length = genes[geneId].rightLimit - genes[geneId].leftLimit + 1;

    char *tmp = new char[length + 2];

    const char *seqRef = ref.getSeq(genes[geneId].tid);

    if (isForward == 1) {
        copy(seqRef + genes[geneId].leftLimit - 1, seqRef + genes[geneId].rightLimit, tmp);
    } else {
        int x = 0;
        for (int j = length - 1; j >= 0; j--) {
            tmp[x++] = getCharComp(seqRef[genes[geneId].leftLimit + j - 1]);
        }
    }
    tmp[length] = '$';
    tmp[length + 1] = '\0';

    // cout<<tmp[0]<<tmp[1]<<tmp[2]<<tmp[3]<<tmp[4]<<"<-->"<<tmp[length-4]<<tmp[length-3]<<tmp[length-2]<<tmp[length-1]<<tmp[length]<<endl;

    /*     SuffixTree sft;
         cout<<"create"<<endl;
         sft.create(tmp,length+1);
         cout<<"copy"<<endl;
         sft.copyThings();
         cout<<"travel"<<endl;
         sft.traverseNodePos();
         sft.printThings();
  */
    SuffixArray2 sfa;
    // cout<<"getArray"<<endl;
    sfa.builtArray(tmp, length + 1);

    if (isForward == 1) {
        //	cout<<"build"<<endl;
        bwts[geneId].create(tmp, length + 1, &sfa);
        //	cout<<"get"<<endl;
        bwts[geneId].getOccAndOB(tmp, length + 1);
    } else {
        //	cout<<"build"<<endl;
        rbwts[geneId].create(tmp, length + 1, &sfa);
        //	cout<<"get"<<endl;
        rbwts[geneId].getOccAndOB(tmp, length + 1);
    }

    delete[] tmp;

    return 0;
}

int Gene::allocate() {
    bwts = new BWT[genes.size()];
    rbwts = new BWT[genes.size()];
    return 0;
}

int Gene::buildALLSuffix(Reference &ref) {

    float t = clock();
    bwts = new BWT[genes.size()];
    rbwts = new BWT[genes.size()];
    int x = 0;
    for (int i = 0; i < genes.size(); i++) {
        x++;
        buildOneSuffix(i, 1, ref);
        buildOneSuffix(i, 0, ref);

        if (x % 100 == 0) {
            cout << (clock() - t) / CLOCKS_PER_SEC << " seconds" << endl;
            t = clock();
        }
    }

    /*
            int totalLen=0;


    float t1=0.0;
    float t2=0.0;
    float t3=0.0;
    float t4=0.0;
    float t5=0.0;

            for(int i=0;i<genes.size();i++)
            {
                    cout<<"Gene "<<i<<endl;
                    cout<<"tid= "<<genes[i].tid<<endl;
                    cout<<genes[i].leftLimit<<" "<<genes[i].rightLimit<<endl;

                    int length=genes[i].rightLimit-genes[i].leftLimit+1;
                    char *tmp=new char [length+2];

                    totalLen+=length;

                    cout<<"currently"<<totalLen<<endl;

                    for(int j=0;j<length;j++)
                    {
                            uint32_t refPos=ref.to_ref_pos(genes[i].tid, genes[i].leftLimit+j);
                            tmp[j]=ref.getRefChar(refPos);
                    }
                    tmp[length]='$';
                    tmp[length+1]='\0';


                    char * rtmp=new char [length+2];

                    int x=0;
                    for(int j=length-1;j>=0;j--)
                    {
                            rtmp[x++]=getCharComp(tmp[j]);
                    }

                    rtmp[length]='$';
                    rtmp[length+1]='\0';

    cout<<tmp[0]<<tmp[1]<<tmp[2]<<tmp[3]<<tmp[4]<<"<-->"<<tmp[length-4]<<tmp[length-3]<<tmp[length-2]<<tmp[length-1]<<tmp[length]<<endl;
    cout<<rtmp[0]<<rtmp[1]<<rtmp[2]<<rtmp[3]<<rtmp[4]<<"<-->"<<rtmp[length-4]<<rtmp[length-3]<<rtmp[length-2]<<rtmp[length-1]<<rtmp[length]<<endl;


            cout<<"got tmp"<<endl;
            float t=clock();

            SuffixTree sft;
            cout<<"Create"<<endl;
            sft.create(tmp,length+1);
    t1+=(clock()-t);
    cout<<(clock()-t)/CLOCKS_PER_SEC<<" seconds"<<endl;t=clock();
                    cout<<"copy"<<endl;
                    sft.copyThings();
    t2+=(clock()-t);
    cout<<(clock()-t)/CLOCKS_PER_SEC<<" seconds"<<endl;t=clock();
                    cout<<"Travel"<<endl;
                    sft.traverseNodePos();
    t3+=(clock()-t);
    cout<<(clock()-t)/CLOCKS_PER_SEC<<" seconds"<<endl;t=clock();
                    cout<<"creat Array"<<endl;
                    SuffixArray sfa;
                    sfa.getArray(sft, length+1);
    t4+=(clock()-t);
    cout<<(clock()-t)/CLOCKS_PER_SEC<<" seconds"<<endl;t=clock();
                    cout<<"creat BWT"<<endl;
                    bwts[i].create(tmp,length+1,sfa);
    t5+=(clock()-t);
    cout<<(clock()-t)/CLOCKS_PER_SEC<<" seconds"<<endl;t=clock();


                    delete [] tmp;
            }
    cout<<t1/CLOCKS_PER_SEC<<" "<<t2/CLOCKS_PER_SEC<<" "<<t3/CLOCKS_PER_SEC<<" "<<t4/CLOCKS_PER_SEC<<" "<<t5/CLOCKS_PER_SEC<<endl;
    */
    return 0;
}

BWT *Gene::getBWT(int geneId) { return &(bwts[geneId]); }

int Gene::getStrand(int geneId) { return genes[geneId].strand; }

string Gene::getName2(int geneId) { return genes[geneId].name2; }

BWT *Gene::getRBWT(int geneId) { return &(rbwts[geneId]); }

int Gene::getExonBoundry(int gid, int isbkLeft, vector<uint32_t> &boundry) {

    for (int i = 0; i < genes[gid].transIds.size(); i++) {
        int tranId = genes[gid].transIds[i];
        int count = transcripts[tranId].exonCount;
        for (int j = 0; j < count; j++) {
            if (isbkLeft == 1)
                boundry.push_back(transcripts[tranId].exonStarts[j]);
            else
                boundry.push_back(transcripts[tranId].exonEnds[j]);
        }
    }

    sort(boundry.begin(), boundry.end());
    vector<uint32_t>::iterator it = unique(boundry.begin(), boundry.end());
    boundry.resize(distance(boundry.begin(), it));
    /*
            for(int i=0;i<boundry.size();i++)
            {
                    cout<<boundry[i]<<",";
            }
            cout<<endl;
    */
    return 0;
}

uint32_t Gene::getLimitLeft(int geneId) { return genes[geneId].leftLimit; }

uint32_t Gene::getLimitRight(int geneId) { return genes[geneId].rightLimit; }

int Gene::getIndex(string name, vector<int> &ids) {
    for (int i = 0; i < genes.size(); i++) {
        if (genes[i].name2.compare(name) == 0) {
            ids.push_back(i);
        }
    }
    return 0;
}

bool Gene::isGeneBWTExist(int geneId) {

    if (bwts[geneId].getLength() == 0)
        return false;
    else
        return true;
}

int Gene::getBestExon(int gid, int pos, int isbkLeft, int &is5p, int &tid, int &strand, int &pos1, int &pos2, string &name, int &exonNum) {
    // cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;

    if ((isbkLeft == 1 && genes[gid].strand == 0) || (isbkLeft == 0 && genes[gid].strand == 1))
        is5p = 0;
    else
        is5p = 1;

    tid = genes[gid].tid;
    strand = genes[gid].strand;

    int best = 1000000000;
    int bestExLen = 1000000000;
    // cout<<genes[gid].transIds.size()<<endl;
    for (int i = 0; i < genes[gid].transIds.size(); i++) {
        int tranId = genes[gid].transIds[i];
        int count = transcripts[tranId].exonCount;
        // cout<<count<<endl;
        for (int j = 0; j < count; j++) {

            int pp1 = transcripts[tranId].exonStarts[j];
            int pp2 = transcripts[tranId].exonEnds[j];

            if (isbkLeft == 1) {
                //				cout<<"A "<<pos<<" "<<pp1<<" "<<pp1<<" "<<best<<" "<<bestExLen<<endl;
                //				cout<<abs(pos-pp1)<<endl;
                //				cout<<best<<endl;

                if (abs(pos - pp1) < best || (abs(pos - pp1) == best && pp2 - pp1 + 1 < bestExLen)) {
                    //					cout<<"change"<<endl;
                    name = transcripts[tranId].name;
                    pos1 = pp1;
                    pos2 = pp2;
                    if (strand == 0) {
                        exonNum = j + 1;
                    } else {
                        exonNum = count - j;
                    }
                    best = abs(pos - pp1);
                    bestExLen = pp2 - pp1 + 1;
                }
            } else {
                //				cout<<"B "<<pos<<" "<<pp1<<" "<<pp2<<" "<<best<<" "<<bestExLen<<endl;
                //				cout<<abs(pos-pp2)<<endl;
                //                              cout<<best<<endl;
                if (abs(pos - pp2) < best || (abs(pos - pp2) == best && pp2 - pp1 + 1 < bestExLen)) {
                    //					cout<<"change"<<endl;
                    name = transcripts[tranId].name;
                    pos1 = pp1;
                    pos2 = pp2;
                    if (strand == 0) {
                        exonNum = j + 1;
                    } else {
                        exonNum = count - j;
                    }
                    best = abs(pos - pp2);
                    bestExLen = pp2 - pp1 + 1;
                }
            }
        }
    }
    pos1 = pos1 + 1;
    return 0;
}

// Dec 7, 2015 for peptides and fusion junction

int Gene::getBestDiff(int gid, int pos, int isbkLeft) {
    int best = 1000000000;
    for (int i = 0; i < genes[gid].transIds.size(); i++) {
        int tranId = genes[gid].transIds[i];
        int count = transcripts[tranId].exonCount;
        for (int j = 0; j < count; j++) {
            int pp1 = transcripts[tranId].exonStarts[j];
            int pp2 = transcripts[tranId].exonEnds[j];
            if (isbkLeft == 1 && abs(pos - pp1) < best)
                best = abs(pos - pp1);
            if (isbkLeft == 0 && abs(pos - pp2) < best)
                best = abs(pos - pp2);
        }
    }
    return best;
}

int Gene::isAt5p(int gid, int isbkLeft) {
    int is5p;
    if ((isbkLeft == 1 && genes[gid].strand == 0) || (isbkLeft == 0 && genes[gid].strand == 1))
        is5p = 0;
    else
        is5p = 1;
    return is5p;
}

int Gene::getCodingAndBaseLeft(int tranId, int exonNum, int isbkLeft, int &isCoding, int &baseLeft) {
    // cout<<"name "<<transcripts[tranId].name<<endl;
    // cout<<"isbkLeft "<<isbkLeft<<endl;
    transcript_t tt = transcripts[tranId];
    if (tt.cdsStart == tt.cdsEnd)
        isCoding = 0;
    else
        isCoding = 1;
    baseLeft = -1;
    if (isCoding == 1) {
        int len_seq = 0;
        int diff = 0;
        int number;

        if (transcripts[tranId].strand == 0 && isbkLeft == 0)
            number = exonNum;
        if (transcripts[tranId].strand == 0 && isbkLeft == 1)
            number = exonNum - 1;
        if (transcripts[tranId].strand == 1 && isbkLeft == 1)
            number = transcripts[tranId].exonCount - exonNum;
        if (transcripts[tranId].strand == 1 && isbkLeft == 0)
            number = transcripts[tranId].exonCount - exonNum + 1;
        // cout<<"number = "<<number<<endl;
        for (int i = 0; i < number; i++) {
            len_seq += tt.exonEnds[i] - tt.exonStarts[i];
            if (tt.exonEnds[i] < tt.cdsStart) {
                // cout<<tt.exonEnds[i]<<" XXX "<<tt.exonStarts[i]<<endl;
                diff += tt.exonEnds[i] - tt.exonStarts[i];
                // cout<<"diff = "<<diff<<endl;
            } else if (tt.exonEnds[i] >= tt.cdsStart && tt.exonStarts[i] <= tt.cdsStart) {
                diff += tt.cdsStart - tt.exonStarts[i];
                // cout<<tt.cdsStart<<" YYY "<<tt.exonStarts[i]<<endl;
                // cout<<"diff = "<<diff<<endl;
            }
        }
        if (len_seq - diff >= 0 && tt.cdsEnd > tt.exonEnds[number - 1]) {
            // cout<<"len_seq = "<<len_seq<<" diff = "<<diff<<endl;
            baseLeft = (len_seq - diff) % 3;
            // cout<<"baseLeft "<<baseLeft<<endl;
        }
        if (baseLeft != -1 && isbkLeft == 1) // this !=-1 is OK
        {
            baseLeft = (3 - baseLeft) % 3;
            // cout<<"baseLeft change to "<<baseLeft<<endl;
        }

        if (transcripts[tranId].strand == 0 && isbkLeft == 0 && tt.cdsStart > tt.exonEnds[number - 1])
            baseLeft = -1;
        if (transcripts[tranId].strand == 1 && isbkLeft == 1 && tt.cdsEnd < tt.exonStarts[number - 1])
            baseLeft = -1;
        if (transcripts[tranId].strand == 0 && isbkLeft == 0 && tt.cdsEnd < tt.exonEnds[number - 1])
            baseLeft = -2;
        if (transcripts[tranId].strand == 1 && isbkLeft == 1 && tt.cdsStart > tt.exonStarts[number - 1])
            baseLeft = -2;
        // cout<<"baseLeft final to "<<baseLeft<<endl;
    }

    return 0;
}

junction_t assign_junction(int gId, int is5p, int tid, int strand, int pos1, int pos2, int exonNum, int is_coding, int baseLeft, string name, int coding_start) {

    junction_t jt;
    jt.gId = gId;
    jt.is5p = is5p;
    jt.isCoding = is_coding;
    jt.coding_start = coding_start;
    jt.coding_left = baseLeft;
    jt.tid = tid;
    jt.strand = strand;
    jt.pos1 = pos1;
    jt.pos2 = pos2;
    jt.exonNum = exonNum;
    jt.name = name;

    return jt;
}

int Gene::getBestExon2(int gid, int pos, int isbkLeft, vector<junction_t> &juncs) {

    int is5p, tid, strand, pos1, pos2, exonNum;
    int is_coding;
    int baseLeft;
    string name;

    int best = getBestDiff(gid, pos, isbkLeft);
    is5p = isAt5p(gid, isbkLeft);

    tid = genes[gid].tid;
    strand = genes[gid].strand;

    for (int i = 0; i < genes[gid].transIds.size(); i++) {
        int tranId = genes[gid].transIds[i];
        int count = transcripts[tranId].exonCount;

        // int seq_len = 0;

        for (int j = 0; j < count; j++) {

            int pp1 = transcripts[tranId].exonStarts[j];
            int pp2 = transcripts[tranId].exonEnds[j];

            // seq_len += pp2 - pp1;

            if (isbkLeft == 1) {
                if (abs(pos - pp1) == best) {
                    name = transcripts[tranId].name;
                    pos1 = pp1;
                    pos2 = pp2;
                    if (strand == 0) {
                        exonNum = j + 1;
                    } else {
                        exonNum = count - j;
                    }
                    getCodingAndBaseLeft(tranId, exonNum, isbkLeft, is_coding, baseLeft);
                    int coding_start = 0;
                    if (is5p == 1 && is_coding == 1 && baseLeft >= 0) {
                        if (transcripts[tranId].cdsEnd >= pos1 && transcripts[tranId].cdsEnd <= pos2)
                            coding_start = pos2 - transcripts[tranId].cdsEnd + 1;
                        else
                            coding_start = ((pos2 - pos1) - baseLeft) % 3 + 1;
                    }
                    // cout<<"push baseLeft "<<baseLeft<<endl;
                    juncs.push_back(assign_junction(gid, is5p, tid, strand, pos1, pos2, exonNum, is_coding, baseLeft, name, coding_start));
                }

            } else {

                if (abs(pos - pp2) == best) {
                    name = transcripts[tranId].name;
                    pos1 = pp1;
                    pos2 = pp2;
                    if (strand == 0) {
                        exonNum = j + 1;
                    } else {
                        exonNum = count - j;
                    }

                    getCodingAndBaseLeft(tranId, exonNum, isbkLeft, is_coding, baseLeft);
                    int coding_start = 0;
                    if (is5p == 1 && is_coding == 1 && baseLeft >= 0) {
                        if (transcripts[tranId].cdsStart >= pos1 && transcripts[tranId].cdsStart <= pos2)
                            coding_start = transcripts[tranId].cdsStart + 1 - pos1;
                        else
                            coding_start = ((pos2 - pos1) - baseLeft) % 3 + 1;
                    }
                    // cout<<"push2 baseLeft "<<baseLeft<<endl;
                    juncs.push_back(assign_junction(gid, is5p, tid, strand, pos1, pos2, exonNum, is_coding, baseLeft, name, coding_start));
                }
            }
        }
    }

    return 0;
}

int Gene::getStrandnPrimenTid(int gid, int isbkLeft, int &is5p, int &tid, int &strand) {
    if ((isbkLeft == 1 && genes[gid].strand == 0) || (isbkLeft == 0 && genes[gid].strand == 1))
        is5p = 0;
    else
        is5p = 1;

    tid = genes[gid].tid;
    strand = genes[gid].strand;
    return 0;
}
