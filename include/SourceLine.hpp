#pragma once
#include <filesystem>
#include <string>

struct SourceLine {
    std::u8string line;
    size_t lineIndexInFile;
    std::filesystem::path filePath;
};