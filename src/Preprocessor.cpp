#include "Preprocessor.hpp"
#include "ErrorHandler.hpp"

static std::vector<std::filesystem::path> s_includedFiles;

static std::u8string readFileToU8String(std::filesystem::path& filePath) {
    std::ifstream file = std::ifstream(filePath, std::ios::binary);  // Open file in binary mode
    assert(file && "File doesn't exist");

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::u8string(content.begin(), content.end());  // Convert to u8string
}


static bool isBody = true;
static size_t depthVal = 0;
void processPreprocessors(std::filesystem::path& mainFilePath, std::u8string& outStr, std::vector<std::filesystem::path>& includingStack, std::vector<std::filesystem::path>& outLibraries) {
    

    
    s_includedFiles.push_back(mainFilePath);
    includingStack.push_back(mainFilePath);

    outStr = readFileToU8String(mainFilePath);

    std::string depthVals = std::to_string(depthVal);
    std::u8string depthCount(depthVals.begin(), depthVals.end());
    std::u8string depth = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH" + depthCount;
    std::u8string depthEnd = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH end" + depthCount;
    depthMapping(mainFilePath.generic_u8string());

    // size_t counter = 0;
    // while(outStr.at(counter) != '\n'){
    //     counter++;
    // }
    for (size_t i = 0; i < outStr.length(); i++){
        if(outStr.at(i) == '\n'){
            outStr.insert(i, depth);
            break;
        }
    }


    outStr = outStr + depthEnd;

    depthVal++;

    const std::u8string INCLUDE = u8"apere";
    size_t pos = outStr.find(INCLUDE, 0);

    //for ErrorHandler
    // bool existPos = pos != std::u8string::npos;
    // if(!existPos){
    //     setFile(mainFilePath.generic_u8string(), outStr, 0, existPos);
    // } else {
    //     setFile(mainFilePath.generic_u8string(), outStr, pos, existPos);
    // }
    
    // if(isBody){
    //     isBody = false;
    // }
    
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

        index = pos;


        // insert file if it's not included yet
        try {
            std::filesystem::path includedPath = std::filesystem::canonical(
                mainFilePath.parent_path() / std::filesystem::path(includeFileName)
            );

            // Check if there is a circle in inclusion
            if (std::find(includingStack.begin(), includingStack.end(), includedPath) != includingStack.end()) {
                assert(false && "Recursive including found");
            }
            
            // Don't include files that are already included
            if (std::find(s_includedFiles.begin(), s_includedFiles.end(), includedPath) == s_includedFiles.end()) {
                auto extension = includedPath.extension();
                if (extension == ".lorem") {
                    std::u8string includedCode;
                    processPreprocessors(includedPath, includedCode, includingStack, outLibraries); 
                    outStr.insert(pos, includedCode);
                    index += includedCode.length();
                } else if (extension == ".a" || extension == ".lib") {
                    outLibraries.push_back(includedPath);
                    s_includedFiles.push_back(includedPath);
                }
            } 
        } catch (const std::filesystem::filesystem_error& e) {
            assert(false && "File doesn't exists");
        }

        pos = outStr.find(INCLUDE, index);
    }
    
    assert(includingStack.back() == mainFilePath);
    includingStack.pop_back();
}
