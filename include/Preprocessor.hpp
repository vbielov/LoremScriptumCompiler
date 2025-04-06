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
    std::unique_ptr<LoremSourceFile> m_rootFile;
    std::vector<std::filesystem::path> m_includedFiles;
    std::vector<std::filesystem::path> m_linkLibraries;

public:
    Preprocessor(std::filesystem::path& mainFilePath);
    std::u8string getMergedSourceCode() const;
    const std::vector<std::filesystem::path>& getLinkLibs() const;
    const LoremSourceFile* findFileFromLineInMergedCode(size_t line) const;

private:
    const LoremSourceFile* findFile(size_t line, const LoremSourceFile& file) const;
    std::unique_ptr<LoremSourceFile> createFileTree(std::filesystem::path filePath, std::vector<std::filesystem::path>& includingStack);
    std::u8string mergeSourceCode(LoremSourceFile& file) const;
    static std::u8string readFileToU8String(std::filesystem::path& filePath);

};
