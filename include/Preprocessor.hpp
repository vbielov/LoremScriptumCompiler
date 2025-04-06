#pragma once
#include <vector>
#include <sstream>
#include <filesystem>
#include <codecvt>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <stack>
#include "ErrorHandler.hpp"

class Preprocessor {
private:
    std::vector<FileRange> m_includedFiles;
    std::vector<std::filesystem::path> m_linkLibraries;
    size_t m_depthVal;

public:
    Preprocessor();
    std::u8string process(std::filesystem::path& mainFilePath);
    const std::vector<std::filesystem::path>& getLinkLibs() const;
    const std::vector<FileRange>& getIncludedFiles() const;

private:
    void processRecursively(std::filesystem::path& mainFilePath, std::u8string& outStr, std::vector<std::filesystem::path>& includingStack);
    static std::u8string readFileToU8String(std::filesystem::path& filePath);

};
