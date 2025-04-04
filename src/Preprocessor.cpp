#include "Preprocessor.hpp"

Preprocessor::Preprocessor()
    : m_includedFiles()
    , m_linkLibraries()
    , m_depthVal(0) {}
    
std::u8string Preprocessor::process(std::filesystem::path& mainFilePath) {
    m_includedFiles.clear();
    std::u8string sourceCode;
    std::vector<std::filesystem::path> stack;
    processRecursively(mainFilePath, sourceCode, stack);

    return sourceCode;
}

const std::vector<std::filesystem::path>& Preprocessor::getLinkLibs() const {
    return m_linkLibraries;
}

void Preprocessor::processRecursively(std::filesystem::path& mainFilePath, std::u8string& outStr, std::vector<std::filesystem::path>& includingStack) {
    m_includedFiles.push_back(mainFilePath);
    includingStack.push_back(mainFilePath);

    outStr = readFileToU8String(mainFilePath);

    //ErrorHandler, markers
    std::string depthVals = std::to_string(m_depthVal);
    std::u8string depthCount(depthVals.begin(), depthVals.end());
    std::u8string depth = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH" + depthCount;
    std::u8string depthEnd = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH end" + depthCount;
    depthMapping(mainFilePath.generic_u8string());

    bool setEnd = false;
    for (size_t i = 0; i < outStr.length(); i++){
        if(outStr.at(i) == '\n'){
            outStr.insert(i, depth);
            setEnd=true;
            break;
        }
    }

    if(!setEnd){
        outStr.append(depth);
    }

    outStr = outStr + depthEnd;
    m_depthVal++;

    const std::u8string INCLUDE = u8"apere";
    size_t pos = outStr.find(INCLUDE, 0);
    
    while (pos != std::u8string::npos) {
        int indexBack = pos - 1;
        bool valid = true;
        while (outStr[indexBack] != u8'\n' && indexBack > 0) {
            if (outStr[indexBack] != u8' ' && outStr[indexBack] != u8'\t') {
                valid = false;
                break;
            }
            indexBack--;
        }
        if (!valid) {
            pos = outStr.find(INCLUDE, pos + INCLUDE.length());
            continue;
        }

        size_t index = pos + INCLUDE.length();
        // skip all spaces, tabs
        while (outStr[index] == ' ' || outStr[index] == '\t')
            index++;

        if(outStr[index] != '\''){
            dumpAndBuildError(u8"Expected ' after apere");
        }
        
        assert(outStr[index] == '\'' && "Expected ' after apere");
        index++; // eat '
        std::string includeFileName = "";
        while (outStr[index] != '\'' && index < outStr.length()) {
            includeFileName += outStr[index];
            index++;
        }

        if(outStr[index] != '\''){dumpAndBuildError(u8"Found no closing ' in apere");}
        assert(outStr[index] == '\'' && "Found no closing ' in apere");
        index++; // eat '

        outStr.erase(pos, index - pos); // remove appere

        index = pos;

        // insert file if it's not included yet
        std::filesystem::path includedPath = mainFilePath.parent_path() / std::filesystem::path(includeFileName);

        // Check if there is a circle in inclusion
        if (std::find(includingStack.begin(), includingStack.end(), includedPath) != includingStack.end()) {
            
            dumpAndBuildError(u8"Recursive including found");
            assert(false && "Recursive including found");
        }
        
        // Don't include files that are already included
        if (std::find(m_includedFiles.begin(), m_includedFiles.end(), includedPath) == m_includedFiles.end()) {
            auto extension = includedPath.extension();
            if (extension == ".lorem") {
                std::u8string includedCode;
                processRecursively(includedPath, includedCode, includingStack); 
                outStr.insert(pos, includedCode);
                index += includedCode.length();
            } else if (extension == ".a" || extension == ".lib" || extension == ".o") {
                m_linkLibraries.push_back(includedPath);
                m_includedFiles.push_back(includedPath);
            }
        } 

        pos = outStr.find(INCLUDE, index);
    }
    
    if(includingStack.back() != mainFilePath){
        dumpAndBuildError(u8"Import failure: includingStack.back() != mainFilePath");
    }
    assert(includingStack.back() == mainFilePath);
    includingStack.pop_back();
}

std::u8string Preprocessor::readFileToU8String(std::filesystem::path& filePath) {
    std::ifstream file = std::ifstream(filePath, std::ios::binary);  // Open file in binary mode
    if (!file) {
        std::string pathStr = filePath.string();
        dumpAndBuildError(u8"File " + std::u8string(pathStr.begin(), pathStr.end()) + u8" isn't found");
        assert(false);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::u8string(content.begin(), content.end());  // Convert to u8string
}
