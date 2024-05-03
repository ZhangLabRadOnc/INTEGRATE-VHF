#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "VirusLoader.h"

using namespace std;

namespace fs = std::filesystem;

VirusLoader::VirusLoader(const string &indexFilePath, const unordered_map<string, string> &virusTypes)
{
    ifstream indexFile(indexFilePath);
    string line;

    while (getline(indexFile, line))
    {
        size_t iPos = line.find('=');
        if (iPos != string::npos)
        {
            string key = line.substr(0, iPos);
            string value = line.substr(iPos + 1);
            string originalName, mappedName = key, filePathStr, refPathStr, annotPathStr;
            iPos = value.find(' ');
            if (iPos != string::npos)
            {
                originalName = value.substr(0, iPos);
                filePathStr = value.substr(iPos + 1);
                iPos = filePathStr.find(' ');
                if (iPos != string::npos)
                {
                    refPathStr = filePathStr.substr(0, iPos);
                    annotPathStr = filePathStr.substr(iPos + 1);
                } else {
                    annotPathStr = filePathStr;
                }
            } else {
                originalName = key;
                annotPathStr = value;
            }

            if (!refPathStr.empty()) {
                fs::path refPath = fs::path(refPathStr);
                if (!fs::exists(refPath))
                {
                    refPath = fs::path(indexFilePath).parent_path() / refPathStr;
                    if (!fs::exists(refPath))
                    {
                        cerr << "Virus reference file does not exist: " << refPathStr << endl;
                        continue;
                    } else {
                        refPathStr = refPath.string();
                    }
                }
            }

            if (!annotPathStr.empty())
            {
                fs::path annotPath = fs::path(annotPathStr);
                if (!fs::exists(annotPath))
                {
                    annotPath = fs::path(indexFilePath).parent_path() / annotPathStr;
                    if (!fs::exists(annotPath))
                    {
                        cerr << "Virus annotation file does not exist: " << annotPathStr << endl;
                        continue;
                    } else {
                        annotPathStr = annotPath.string();
                    }
                }
            }

            if (this->virusMap.find(mappedName) == this->virusMap.end())
            {
                this->virusMap[mappedName] = VirusNameFile(originalName, refPathStr, annotPathStr);
            } else {
                cerr << "Duplicate virus name: " << mappedName << endl;
                continue;
            }
        }
    }
    indexFile.close();

    for (const auto &pair : virusTypes)
    {
        string filePathStr = pair.second;
        fs::path filePath = fs::path(filePathStr);
        if (!fs::exists(filePath))
        {
            filePath = fs::path(indexFilePath).parent_path() / filePathStr;
            if (!fs::exists(filePath))
            {
                cerr << "Virus type file does not exist: " << filePathStr << endl;
                continue;
            }
        }
        if (pair.first == "HPVEM")
        {
            this->loadHPVEM(filePath);
        }
        else
        {
            cerr << "Unknown virus result type: " << pair.first << endl;
        }
    }
}

void VirusLoader::loadHPVEM(const fs::path &filePath)
{
    cout << "Loading HPV-EM result from: " << filePath << endl;
    ifstream hpvFile(filePath);
    string line;
    bool headerFound = false;
    while (getline(hpvFile, line))
    {
        if (line.length() > 0)
        {
            if (!headerFound)
            {
                if (line.starts_with("HPVtype"))
                {
                    headerFound = true;
                    continue;
                }
            }
            else
            {
                line = regex_replace(line, regex("\t"), " ");
                stringstream ss(line);
                string hpv;
                getline(ss, hpv, ' ');
                bool found = false;
                cout << "Loading HPV type: " << hpv << "" << endl;
                for (const auto &pair : this->virusMap)
                {
                    if (pair.first == hpv)
                    {
                        found = true;
                        if (!pair.second.refPath.empty()) {
                            if (this->selRefMap.find(pair.second.refPath) == selRefMap.end())
                            {
                                this->selRefMap[pair.second.refPath] = vector<VirusLoader::VirusNamePair>();
                            }
                            this->selRefMap[pair.second.refPath].push_back(VirusNamePair(pair.second.originalName, pair.first));
                        }
                        if (!pair.second.annotPath.empty())
                        {
                            if (this->selAnnotMap.find(pair.second.annotPath) == selAnnotMap.end())
                            {
                                this->selAnnotMap[pair.second.annotPath] = vector<VirusLoader::VirusNamePair>();
                            }
                            this->selAnnotMap[pair.second.annotPath].push_back(VirusNamePair(pair.second.originalName, pair.first));
                        }
                        cout << "Found HPV type: " << pair.first << " as " << pair.second.originalName << endl;
                        if (!pair.second.refPath.empty())
                        {
                            cout << pair.second.refPath << endl;
                        }
                        if (!pair.second.annotPath.empty())
                        {
                            cout << pair.second.annotPath << endl;
                        }
                        break;
                    }
                }
                if (!found)
                {
                    cerr << "Unknown HPV type: " << hpv << endl;
                }
            }
        }
    }
    hpvFile.close();
}
