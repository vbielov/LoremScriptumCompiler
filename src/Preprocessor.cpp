#include "Preprocessor.hpp"

static std::vector<std::filesystem::path> s_includedFiles;

static std::u8string readFileToU8String(std::filesystem::path& filePath) {
    std::ifstream file = std::ifstream(filePath, std::ios::binary);  // Open file in binary mode
    assert(file && "File doesn't exist");

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::u8string(content.begin(), content.end());  // Convert to u8string
}

void processPreprocessors(std::filesystem::path& mainFilePath, std::u8string& outStr) {
    s_includedFiles.push_back(mainFilePath);
    outStr = readFileToU8String(mainFilePath);

    const std::u8string INCLUDE = u8"apere";
    size_t pos = outStr.find(INCLUDE, 0);
    while (pos != std::u8string::npos) {
        size_t index = pos + INCLUDE.length();
        // skip all spaces, tabs
        while (outStr[index] == ' ' || outStr[index] == '\t')
            index++;

        assert(outStr[index] == '\'' && "Expected ' after apere");
        index++; // eat '
        std::string includeFileName = "";
        while (outStr[index] != '\'' && index < outStr.length()) {
            includeFileName += outStr[index];
            index++;
        }
        assert(outStr[index] == '\'' && "Found no closing ' in apere");
        index++; // eat '

        outStr.erase(pos, index - pos); // remove appere
        
        // insert file if it's not included yet
        try {
            std::filesystem::path includedPath = std::filesystem::canonical(
                mainFilePath.parent_path() / std::filesystem::path(includeFileName)
            );
            if (std::find(s_includedFiles.begin(), s_includedFiles.end(), includedPath) == s_includedFiles.end()) {
                std::u8string includedCode;
                processPreprocessors(includedPath, includedCode); 
                outStr.insert(pos, includedCode);
            } else {
                assert(false && "Recursive including found");
            }
        } catch (const std::filesystem::filesystem_error& e) {
            assert(false && "File doesn't exists");
        }

        pos = outStr.find(INCLUDE, index + 1);
    }
}
