#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;

namespace fs = std::filesystem;

class VirusLoader {
public:
    struct VirusNamePair {
        string originalName;
        string mappedName;
        VirusNamePair() = default;
        VirusNamePair(string originalName, string mappedName) : originalName(originalName), mappedName(mappedName) {}
    };
private:
    struct VirusNameFile {
        string originalName;
        string gffName;
        string gffPath;
        string fastaName;
        string fastaPath;
        string annotName;
        string annotPath;
        VirusNameFile() = default;
        VirusNameFile(string originalName, string gffName, string gffPath, string fastaName, string fastaPath, string annotName, string annotPath) :
            originalName(originalName), gffName(gffName), gffPath(gffPath), fastaName(fastaName), fastaPath(fastaPath), annotName(annotName), annotPath(annotPath) {}
    };
    void loadHPVEM(const fs::path &filePath);
public:
    unordered_map<string, VirusNameFile> virusMap;
    vector<tuple<string, string, string>> selRefs, selAnnots;
    VirusLoader(const string &indexFilePath, const unordered_map<string, string> &virusTypes);
};
