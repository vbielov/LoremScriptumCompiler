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
    auto lines = mergeSourceCode(m_rootFile.get());
    std::u8string mergedCode = u8"";
    for (const auto& line : lines) {
        mergedCode += std::get<1>(line);
    }

    for(size_t i = 0; i < lines.size(); i++) {
        std::cout << "Line " << i << " Merged Line " << std::get<2>(lines[i]) << " in file " << std::get<0>(lines[i])->filePath.filename() << ": " << (const char*)(std::get<1>(lines[i]).c_str()) << std::endl;
    } 

    return mergedCode;
}

const std::vector<std::filesystem::path>& Preprocessor::getLinkLibs() const {
    return m_linkLibraries;
}

const LoremSourceFile* Preprocessor::findFileFromLineInMergedCode(size_t line) const {
    return findFile(line, *m_rootFile);
}

const LoremSourceFile* Preprocessor::findFile(size_t line, const LoremSourceFile& file) const {
    size_t lineInTheFile = line;
    for(const auto& includedFile : file.includedLorem) {
        size_t pos = includedFile.first;
        if (line < pos) {
            break;
        }
        const auto& file = includedFile.second;

        if (line >= pos && line < pos + file->sourceCode.length()) {
            return findFile(line - pos, *file.get());
        }
        lineInTheFile -= file->sourceCode.length();
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


std::vector<std::tuple<const LoremSourceFile*, std::u8string, size_t>> Preprocessor::mergeSourceCode(const LoremSourceFile* file) const {
    assert(file);
    std::vector<std::tuple<const LoremSourceFile*, std::u8string, size_t>> lines;
    lines.emplace_back(file, std::u8string(), 0);

    const std::u8string& code = file->sourceCode;
    for (size_t i = 0; i < code.length(); i++) {
        std::get<1>(lines.back()) += code[i];

        if (code[i] == u8'\n') {
            lines.emplace_back(file, std::u8string(), lines.size());
        }
    }
    
    size_t offset = 0;
    for (const auto& includedFile : file->includedLorem) {
        size_t line = includedFile.first;
        const auto& file = includedFile.second;

        // Recursively merge the source code of the included file
        auto fileLines = mergeSourceCode(file.get());
        lines.insert(lines.begin() + line + offset, fileLines.begin(), fileLines.end());
        offset += fileLines.size() - 1; // Adjust the offset for the next included file
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
