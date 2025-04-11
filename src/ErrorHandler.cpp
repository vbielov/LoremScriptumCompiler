#include "ErrorHandler.hpp"

ErrorHandler* ErrorHandler::getInstance() {
    if(s_instance == nullptr) {
        std::lock_guard<std::mutex> lock(s_mutex); // Lock the mutex to ensure thread safety
        if(s_instance == nullptr) { // Double-check if instance is still null
            s_instance = new ErrorHandler(); // Create the singleton instance
        }
    }
    return s_instance; // Return the singleton instance
}

void ErrorHandler::init(const std::vector<SourceLine>& lines) {
    getInstance()->m_sourceLines = &lines; // Initialize the source lines reference
}

bool ErrorHandler::hasError() {
    return getInstance()->m_errorFlag;
}

bool ErrorHandler::hasWarning() {
    return getInstance()->m_warnFlag;
}

void ErrorHandler::logError(std::u8string reason, size_t line) {
    auto& instance = *getInstance(); // Get the singleton instance
    instance.m_errorFlag = true;
    instance.log(&line, reason, true); // Log the error
}

void ErrorHandler::logError(std::u8string reason) {
    auto& instance = *getInstance(); // Get the singleton instance
    instance.m_errorFlag = true;
    instance.log(nullptr, reason, true); // Log the error without a specific line
}

void ErrorHandler::logWarning(std::u8string reason, size_t line) {
    auto& instance = *getInstance(); // Get the singleton instance
    instance.m_warnFlag = true;
    instance.log(&line, reason, false); // Log the warning
}

void ErrorHandler::logWarning(std::u8string reason) {
    auto& instance = *getInstance(); // Get the singleton instance
    instance.m_warnFlag = true;
    instance.log(nullptr, reason, false); // Log the warning without a specific line
}

void ErrorHandler::log(size_t* line, std::u8string reason, bool isError) {
    const SourceLine* sourceLine = nullptr; // Initialize sourceLine pointer to null
    if (line) {
        assert(m_sourceLines && "Source lines not initialized!"); // Ensure source lines are initialized
        assert(*line - 1 < m_sourceLines->size());
        sourceLine = &(*m_sourceLines)[*line - 1]; // Get the source lines reference
    }

    const static std::string_view ERROR_STR = "\n \033[1;41mError\033[0m encountered in Line ";
    const static std::string_view WARNING_STR = "\n \033[1;48;5;214mWarning\033[0m for Line ";
    
    std::string outputStr;
    std::stringstream outputStream(outputStr);
    outputStream << (isError ? ERROR_STR : WARNING_STR); // Set the error or warning string
    if (sourceLine) {
        outputStream << "\x1b]8;;vscode://file/"+ sourceLine->filePath.string() << ":" << std::to_string(sourceLine->lineIndexInFile + 1); // link
        outputStream << "\x1b\\" << (const char*)toRomanConverter(sourceLine->lineIndexInFile + 1).c_str() << "\x1b]8;;\x1b\\" << " in File: "+ sourceLine->filePath.string() << "\n"; // link title
        outputStream << "\t \033[31m:" << (const char*)(sourceLine->line.c_str()) << "\033[0m \n"; // line content
    } else {
        outputStream << "In undefined line"; // Unknown file case
    }

    outputStream << "possible Reason: " << (const char*)reason.c_str() << std::endl; // reason 
    std::cerr << outputStream.str() << std::endl;
}
