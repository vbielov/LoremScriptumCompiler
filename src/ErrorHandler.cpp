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

void ErrorHandler::init(const std::vector<SourceLine>& lines, std::filesystem::path mainFilePath) {
    getInstance()->m_sourceLines = &lines; // Initialize the source lines reference

    std::string disclaimer = "----------------------- Error/s or Warning/s while compiling  " + mainFilePath.string() + " -----------------------\n bracket errors can be very inconsistant with more errors mixed in, for ease of use consider using our vscode extention for highlighting\n\n";
    
    if(!(getInstance()->m_queueFlag)){
        std::cout << disclaimer;
    } else {
        std::string preProcessorErrors = getInstance()->m_ErrorQueue;
        getInstance()->m_ErrorQueue = disclaimer;
        getInstance()->m_ErrorQueue.append(preProcessorErrors);
    }
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

    const SourceLine* previousLine = nullptr;
    const SourceLine* followingLine = nullptr;

    if (line) {
        if(!m_sourceLines) {
            return; // Ensure source lines are initialized. Can't assert, because of testing
        } else  {
            assert(*line - 1 < m_sourceLines->size());
            sourceLine = &(*m_sourceLines)[*line - 1]; // Get the source lines reference
            std::filesystem::path mainFilePath = sourceLine->filePath;

            if (*line - 2 < m_sourceLines->size()) {
                previousLine = &(*m_sourceLines)[*line - 2];
            
                if (previousLine->filePath != mainFilePath){
                    previousLine = nullptr;
                }
            }

            if (*line < m_sourceLines->size()) {
                followingLine = &(*m_sourceLines)[*line];

                if (followingLine->filePath != mainFilePath){
                    followingLine = nullptr;
                }
            }
        }
    }

    std::string outputStr;
    std::stringstream outputStream(outputStr);

    if(!sourceLine){ // handle generic Warning/Error without lines
        if (isError) {
            outputStream << "\n \033[1;41mError\033[0m: " << (const char*)reason.c_str() << std::endl;
        } else {
            outputStream << "\n \033[1;48;5;214mWarning\033[0m: " << (const char*)reason.c_str() << std::endl;
        }
        
    } else {    
        const static std::string_view ERROR_STR = "\n \033[1;41mError\033[0m encountered in Line ";
        const static std::string_view WARNING_STR = "\n \033[1;48;5;214mWarning\033[0m for Line ";
        
        outputStream << (isError ? ERROR_STR : WARNING_STR); // Set the error or warning string
        if (sourceLine) {

            outputStream << "\x1b]8;;vscode://file/"+ sourceLine->filePath.string() << ":" << std::to_string(sourceLine->lineIndexInFile + 1); // link
            outputStream << "\x1b\\" << (const char*)toRomanConverter(sourceLine->lineIndexInFile + 1).c_str() << "\x1b]8;;\x1b\\" << " in File: "+ sourceLine->filePath.string() << "\n"; // link title


            if(previousLine){
                outputStream << "\t | " << (const char*)(previousLine->line.c_str());
            }

            if (isError) {
                outputStream << "\t \033[31m| " << (const char*)(sourceLine->line.c_str()) << "\033[0m"; // line content, in RED
            } else {
                outputStream << "\t \033[38;5;214m| " << (const char*)(sourceLine->line.c_str()) << "\033[0m"; // line content, in ORANGE
            }

            if(followingLine){
                outputStream << "\t | " << (const char*)(followingLine->line.c_str());
            }

        } else {
            outputStream << "In undefined line"; // Unknown file case
        }

        if (isError){
            outputStream << " possible Reason: " << (const char*)reason.c_str() << std::endl; // reason     
        } else {
            outputStream << " Warning: " << (const char*)reason.c_str() << std::endl; 
        }
    }

    
    if (m_queueFlag) {
        outputStream << std::endl;
        if(isError) {
            m_ErrorQueue.append(outputStream.str());
        } else {
            m_WarningQueue.append(outputStream.str());
        }
    } else {
        std::cerr << outputStream.str() << std::endl;
    }   
}

void ErrorHandler::dumpErrorAndWarning(){
    if(getInstance()->m_queueFlag){
        std::cerr << getInstance()->m_ErrorQueue;
        std::cout << getInstance()->m_WarningQueue;
    }
};

