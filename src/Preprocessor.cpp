#include "Preprocessor.hpp"

Preprocessor::Preprocessor(std::filesystem::path& mainFilePath)
    : m_rootFile(nullptr)
    , m_includedFiles()
    , m_linkLibraries()
    , m_mergedLines()
{    
    std::vector<std::filesystem::path> stack;
    m_rootFile = createFileTree(mainFilePath, stack);
    m_mergedLines = mergeFiles(m_rootFile.get());
}

const std::vector<SourceLine>& Preprocessor::getMergedLines() const {
    return m_mergedLines;
}

std::u8string Preprocessor::getMergedSourceCode() const {
    std::u8string mergedCode = u8"";
    for (const auto& line : m_mergedLines) {
        mergedCode += line.line;
    }

    // For debugging purposes, print the merged lines
    // for(size_t i = 0; i < lines.size(); i++) {
    //     std::cout   << "Line " << i + 1 
    //                 << " Merged Line " << lines[i].lineIndexInFile + 1 
    //                 << " in file " << lines[i].filePath.filename() << ": " 
    //                 << (const char*)lines[i].line.c_str() << std::endl;
    // } 

    #if !defined(NDEBUG)
    std::cout << "----------------------- Source Code: ----------------------- " << std::endl << std::endl;
    std::cout << (const char*)(mergedCode.c_str()) << std::endl << std::endl;
    #endif

    return mergedCode;
}

const std::vector<std::filesystem::path>& Preprocessor::getLinkLibs() const {
    return m_linkLibraries;
}

std::unique_ptr<LoremSourceFile> Preprocessor::createFileTree(std::filesystem::path& filePath, std::vector<std::filesystem::path>& includingStack) {
    includingStack.push_back(filePath);
    m_includedFiles.emplace_back(filePath);

    std::unique_ptr<LoremSourceFile> currentFile = std::make_unique<LoremSourceFile>();
    currentFile->filePath = filePath; 
    currentFile->sourceCode = readFileToU8String(filePath);
    
    std::u8string& sourceCode = currentFile->sourceCode;
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
        
        sourceCode.erase(includePos, index - includePos); // remove apere "fileName", but not the \n
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
            int line = countLines(sourceCode, index);
            currentFile->includedLorem.emplace_back(line, std::move(includeFile));
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


std::vector<SourceLine> Preprocessor::mergeFiles(const LoremSourceFile* file) {
    assert(file);
    std::vector<SourceLine> lines;

    const std::u8string& code = file->sourceCode;
    size_t lineStart = 0;
    for (size_t i = 0; i < code.length(); i++) {
        if (code[i] == u8'\n') {
            lines.emplace_back(
                std::u8string(code.begin() + lineStart, code.begin() + i + 1), // Include the last character if it's not a newline
                lines.size(), 
                file->filePath
            );
            lineStart = i + 1; // Move to the next line
        }
    }
    // last line (if it doesn't end with a newline)
    lines.emplace_back(
        std::u8string(code.begin() + lineStart, code.begin() + code.length()),
        lines.size(), 
        file->filePath
    );
    
    size_t offset = 0;
    for (const auto& includedFile : file->includedLorem) {
        size_t line = includedFile.first;
        const auto& file = includedFile.second;

        // Recursively merge the source code of the included file
        auto fileLines = mergeFiles(file.get());
        lines.insert(lines.begin() + line + offset, fileLines.begin(), fileLines.end());
        offset += fileLines.size(); // Adjust the offset for the next included file
    }

    return lines;
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

int Preprocessor::countLines(const std::u8string& str, size_t untilPos) {
    int count = 0;
    for (size_t i = 0; i < untilPos; i++) {
        if (str[i] == u8'\n') {
            count++;
        }
    }
    return count;
}
