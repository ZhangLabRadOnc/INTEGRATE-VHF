#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "VirusLoader.h"

using namespace std;

namespace fs = std::filesystem;

VirusLoader::VirusLoader(const string &indexFilePath, const unordered_map<string, string> &virusTypes)
{
    ifstream indexFile(indexFilePath);
    string line, current;

    map<string, map<string, string>> indexFileMap;
    while (getline(indexFile, line)) {
        if (line.empty() || line[0] == ';')
            continue;

        if (line[0] == '[' && line[line.size() - 1] == ']') {
            current = line.substr(1, line.size() - 2);
            continue;
        }

        size_t delimiterPos = line.find('=');
        if (delimiterPos == string::npos) {
            continue;
        }
        string key = line.substr(0, delimiterPos);
        string value = line.substr(delimiterPos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        indexFileMap[current][key] = value;
    }

    for (const auto &pair : indexFileMap) {
        const string &mappedName = pair.first;
        const map<string, string> &values = pair.second;
        if (values.find("NAME") == values.end()) {
            continue;
        }
        const string &originalName = values.at("NAME");
        string gffName, gffPathStr, fastaName, fastaPathStr, annotName, annotPathStr;
        for (const auto &value : values) {
            if (value.first.starts_with("GFF")) {
                gffName = originalName;
                gffPathStr = value.second;
                if (value.first.ends_with("}") && value.first.contains("{")) {
                    gffName = value.first.substr(4, value.first.size() - 5);
                }
            } else if (value.first.starts_with("FASTA")) {
                fastaName = originalName;
                fastaPathStr = value.second;
                if (value.first.ends_with("}") && value.first.contains("{")) {
                    fastaName = value.first.substr(6, value.first.size() - 7);
                }
            } else if (value.first.starts_with("ANNOT")) {
                annotName = originalName;
                annotPathStr = value.second;
                if (value.first.ends_with("}") && value.first.contains("{")) {
                    annotName = value.first.substr(6, value.first.size() - 7);
                }
            }
        }
        if (!gffPathStr.empty()) {
            fs::path gffPath = fs::path(gffPathStr);
            if (!fs::exists(gffPath))
            {
                gffPath = fs::path(indexFilePath).parent_path() / gffPathStr;
                if (!fs::exists(gffPath))
                {
                    cerr << "Virus GFF file does not exist: " << gffPathStr << endl;
                    continue;
                }
                else
                {
                    gffPathStr = gffPath.string();
                }
            }
        }
        if (!fastaPathStr.empty()) {
            fs::path fastaPath = fs::path(fastaPathStr);
            if (!fs::exists(fastaPath))
            {
                fastaPath = fs::path(indexFilePath).parent_path() / fastaPathStr;
                if (!fs::exists(fastaPath))
                {
                    cerr << "Virus FASTA file does not exist: " << fastaPathStr << endl;
                    continue;
                }
                else
                {
                    fastaPathStr = fastaPath.string();
                }
            }
        }
        if (!annotPathStr.empty()) {
            fs::path annotPath = fs::path(annotPathStr);
            if (!fs::exists(annotPath))
            {
                annotPath = fs::path(indexFilePath).parent_path() / annotPathStr;
                if (!fs::exists(annotPath))
                {
                    cerr << "Virus annotation file does not exist: " << annotPathStr << endl;
                    continue;
                }
                else
                {
                    annotPathStr = annotPath.string();
                }
            }
        }

        if (this->virusMap.find(mappedName) == this->virusMap.end()) {
            this->virusMap[mappedName] = VirusNameFile(originalName, gffName, gffPathStr, fastaName, fastaPathStr, annotName, annotPathStr);
        } else {
            cerr << "Duplicate virus name: " << mappedName << endl;
            break;
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
                    if (pair.first.compare(hpv) == 0)
                    {
                        found = true;
                        if (!pair.second.gffPath.empty()) {
                            this->selAnnots.push_back(make_tuple(pair.second.gffPath, pair.second.gffName, pair.second.originalName));
                        }
                        if (!pair.second.fastaPath.empty()) {
                            this->selRefs.push_back(make_tuple(pair.second.fastaPath, pair.second.fastaName, pair.second.originalName));
                        }
                        if (!pair.second.annotPath.empty()) {
                            this->selAnnots.push_back(make_tuple(pair.second.annotPath, pair.second.annotName, pair.second.originalName));
                        }
                        cout << "Found HPV type: " << pair.first << " => " << pair.second.originalName << endl;
                        if (!pair.second.gffPath.empty())
                        {
                            if (pair.second.gffName.compare(pair.second.originalName) == 0) {
                                cout << pair.second.gffName;
                            } else {
                                cout << pair.second.originalName << " => " << pair.second.gffName;
                            }
                            cout << " in file: " << pair.second.gffPath << endl;
                        }
                        if (!pair.second.fastaPath.empty())
                        {
                            if (pair.second.fastaName.compare(pair.second.originalName) == 0) {
                                cout << pair.second.fastaName;
                            } else {
                                cout << pair.second.originalName << " => " << pair.second.fastaName;
                            }
                            cout << " in file: " << pair.second.fastaPath << endl;
                        }
                        if (!pair.second.annotPath.empty())
                        {
                            if (pair.second.annotName.compare(pair.second.originalName) == 0) {
                                cout << pair.second.annotName;
                            } else {
                                cout << pair.second.originalName << " => " << pair.second.annotName;
                            }
                            cout << " in file: " << pair.second.annotPath << endl;
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
