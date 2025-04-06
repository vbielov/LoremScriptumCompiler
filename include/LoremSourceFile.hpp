#pragma once
#include <vector>
#include <filesystem>
#include <string>
#include <memory>
#include <utility>

// It has a tree structure, where each node represents a file and its included files
struct LoremSourceFile {
    std::filesystem::path filePath;
    std::u8string sourceCode; // without apere
    // position (in node's sourceCode) and file that was included
    std::vector<std::pair<size_t, std::unique_ptr<LoremSourceFile>>> includedLorem;
};