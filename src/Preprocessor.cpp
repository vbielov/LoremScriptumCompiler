#include "Preprocessor.hpp"

Preprocessor::Preprocessor(std::filesystem::path& mainFilePath)
    : m_rootFile(nullptr)
    , m_includedFiles()
    , m_linkLibraries() 
{    
    std::vector<std::filesystem::path> stack;
    m_rootFile = createFileTree(mainFilePath, stack);
}

std::u8string Preprocessor::getMergedSourceCode() const {
    return mergeSourceCode(*m_rootFile);
}

const std::vector<std::filesystem::path>& Preprocessor::getLinkLibs() const {
    return m_linkLibraries;
}

const LoremSourceFile* Preprocessor::findFileFromLineInMergedCode(size_t line) const {
    return findFile(line, *m_rootFile);
}

const LoremSourceFile* Preprocessor::findFile(size_t line, const LoremSourceFile& file) const {
    for(const auto& includedFile : file.includedLorem) {
        size_t pos = includedFile.first;
        const auto& file = includedFile.second;

        if (line >= pos && line < pos + file->sourceCode.length()) {
            return findFile(line - pos, *file.get());
        }
    }
    return &file;
}

std::unique_ptr<LoremSourceFile> Preprocessor::createFileTree(std::filesystem::path filePath, std::vector<std::filesystem::path>& includingStack) {
    std::unique_ptr<LoremSourceFile> currentFile = std::make_unique<LoremSourceFile>();
    currentFile->filePath = filePath;
    currentFile->sourceCode = readFileToU8String(filePath);
    std::u8string& sourceCode = currentFile->sourceCode;

    includingStack.push_back(filePath);
    m_includedFiles.emplace_back(filePath);

    static const std::u8string_view INCLUDE_STR = u8"apere";
    size_t includePos = 0;
    while ((includePos = sourceCode.find(INCLUDE_STR, includePos)) != std::u8string::npos) {
        // This checks if the line with 'apere' has only whitespace before it
        {
            int indexBack = includePos - 1;
            bool onlyApereInLine = true;
            while (sourceCode[indexBack] != u8'\n' && indexBack > 0) {
                if (sourceCode[indexBack] != u8' ' && sourceCode[indexBack] != u8'\t') {
                    onlyApereInLine = false;
                    break;
                }
                indexBack--;
            }
            if (!onlyApereInLine) {
                // TODO(Vlad): Error
                includePos = sourceCode.find(INCLUDE_STR, includePos + INCLUDE_STR.length());
                continue;
            }
        }

        size_t index = includePos + INCLUDE_STR.length();

        // Skip whitespace after 'apere'
        while (sourceCode[index] == u8' ' || sourceCode[index] == u8'\t')
        index++;
        
        // Read the file name from " until the next "
        std::string includeFileName = "";
        {
            if (sourceCode[index] != '"') {
                // TODO(Vlad): Error
                includePos = sourceCode.find(INCLUDE_STR, includePos + INCLUDE_STR.length());
                continue;
            }
    
            index++; // eat "
    
            while (sourceCode[index] != '"' && index < sourceCode.length() && sourceCode[index] != '\n') {
                includeFileName += sourceCode[index];
                index++;
            }
    
            if (sourceCode[index] != '"') {
                // TODO(Vlad): Error
                includePos = sourceCode.find(INCLUDE_STR, includePos + INCLUDE_STR.length());
                continue;
            }
            index++; // eat "
        }
        
        sourceCode.erase(includePos, index - includePos); // remove apere "fileName"
        index = includePos; // set index to the start of the line

        std::filesystem::path includePath = std::filesystem::path(filePath.parent_path()) / std::filesystem::path(includeFileName);
        
        // Check if there is a circle in inclusion
        if (std::find(includingStack.begin(), includingStack.end(), includePath) != includingStack.end()) {
            // TODO(Vlad): Error
            includePos = sourceCode.find(INCLUDE_STR, index);
            continue;
        }

        // Check if the file is already included
        if (std::find(m_includedFiles.begin(), m_includedFiles.end(), includePath) != m_includedFiles.end()) {
            includePos = sourceCode.find(INCLUDE_STR, index);
            continue;
        }

        // It's a library, add it to the list of libraries, to be linked later
        auto extension = includePath.extension();
        if (extension == ".a" || extension == ".so" || extension == ".dll") {
            m_linkLibraries.push_back(includePath);
            m_includedFiles.push_back(includePath);
        }
        // It's a file, process it recursively
        else if (extension == ".lorem") {
            auto includeFile = createFileTree(includePath, includingStack);
            currentFile->includedLorem.emplace_back(index, std::move(includeFile));

            sourceCode.insert(index, includeFile->sourceCode);
            index += includeFile->sourceCode.length(); // Move the index to the end of the inserted code
        }
        else {
            // TODO(Vlad): Error
        }

        includePos = sourceCode.find(INCLUDE_STR, index);
    }

    if (includingStack.back() != filePath) {
        // TODO(Vlad): Error
    }
    includingStack.pop_back();
    return currentFile;
}

std::u8string Preprocessor::mergeSourceCode(LoremSourceFile& file) const {
    // std::u8string mergedCode = m_rootFile->sourceCode;
    
    // for (const auto& includedFile : m_rootFile->includedLorem) {
    //     size_t pos = includedFile.first;
    //     const auto& file = includedFile.second;

    //     // Recursively merge the source code of the included file
    //     std::u8string fileCode = mergeSourceCode(*file.get());
    //     mergedCode.insert(pos, fileCode);
    // }

    // return mergedCode;
    return file.sourceCode;
}

std::u8string Preprocessor::readFileToU8String(std::filesystem::path& filePath) {
    std::ifstream file = std::ifstream(filePath, std::ios::binary);  // Open file in binary mode
    if (!file) {
        std::string pathStr = filePath.string();
        // TODO(Vlad): Error
        //dumpAndBuildError(u8"File " + std::u8string(pathStr.begin(), pathStr.end()) + u8" isn't found");
        assert(false);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::u8string(content.begin(), content.end());  // Convert to u8string
}
