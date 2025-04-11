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

// It has a tree structure, where each node represents a file and its included files
struct LoremSourceFile {
    std::filesystem::path filePath;
    std::u8string sourceCode; // without apere
    // position (in node's sourceCode) and file that was included
    std::vector<std::pair<size_t, std::unique_ptr<LoremSourceFile>>> includedLorem;
};

class Preprocessor {
private:
    std::unique_ptr<LoremSourceFile> m_rootFile;
    std::vector<std::filesystem::path> m_includedFiles;
    std::vector<std::filesystem::path> m_linkLibraries;
    std::vector<SourceLine> m_mergedLines;

public:
    Preprocessor(std::filesystem::path& mainFilePath);

    /// @brief This getter allows ErrorHandler to access origin lines of merged lines
    const std::vector<SourceLine>& getMergedLines() const;

    /// @brief This will just create string with all lines in it
    std::u8string getMergedSourceCode() const;

    /// @brief libraries that have to be included by linker to executable
    const std::vector<std::filesystem::path>& getLinkLibs() const;

private:
    std::unique_ptr<LoremSourceFile> createFileTree(std::filesystem::path& filePath, std::vector<std::filesystem::path>& includingStack);
    static std::vector<SourceLine> mergeFiles(const LoremSourceFile* file);
    static std::u8string readFileToU8String(std::filesystem::path& filePath);
    static int countLines(const std::u8string& str, size_t untilPos);
};
