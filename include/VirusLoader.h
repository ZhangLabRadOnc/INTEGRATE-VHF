#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
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
        string refPath;
        string annotPath;
        VirusNameFile() = default;
        VirusNameFile(string originalName, string refPath) : originalName(originalName), refPath(refPath) {}
        VirusNameFile(string originalName, string refPath, string annotPath) : originalName(originalName), refPath(refPath), annotPath(annotPath) {}
    };
    unordered_map<string, VirusNameFile> virusMap;
    void loadHPVEM(const fs::path &filePath);
public:
    unordered_map<string, vector<VirusNamePair>> selRefMap;
    unordered_map<string, vector<VirusNamePair>> selAnnotMap;
    VirusLoader(const string &indexFilePath, const unordered_map<string, string> &virusTypes);
};
