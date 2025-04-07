#pragma once
#include <string>
#include <regex>
#include <vector>
#include <filesystem>
#include <unordered_set>
#include <iostream>
#include "RomanNumber.hpp"
#include <mutex>
#include "LoremSourceFile.hpp"

class ErrorHandler {
private:
    std::vector<std::u8string> m_errorLog;
    std::vector<std::u8string> m_warningLog;
    bool m_errorFlag;
    bool m_warnFlag;
    bool m_instantDump;

    static ErrorHandler* m_instance; // Singleton instance
    static std::mutex m_mutex; // Mutex for thread safety
    ErrorHandler() {} // Private constructor for singleton pattern

public:
    // Deleting copy constructor and assignment operator to prevent copying
    ErrorHandler(const ErrorHandler& obj) = delete;

    /// @brief Static method to get the singleton instance of ErrorHandler
    static ErrorHandler* getInstance();

    static void init(std::u8string& sourceCode, std::u8string fileName, std::u8string fileLocation, bool instantDump);

    /// @brief Allows error handler to print errors and warnings to the console immediately
    static void setInstantDump(bool state);

    /// @return returns bool based on if any errors happened before hand
    static bool hasError();

    /// @return returns bool based on detected warnings
    static bool hasWarning();

    /// @brief splits source into individual lines
    static void grabSource(std::u8string sourceCode, std::string fileLocation);

    /// @brief build string with reason as error message
    /// get line from source file to print aswell
    /// printed via dumpErrorLog
    static void logError(size_t line, std::u8string reason);

    /// @brief works like logError but doesn't stop program flow
    static void logWarning(size_t line, std::u8string reason);

    /// @brief dumps all ErrorLogs into terminal
    static void dumpErrorLog();

    /// @brief removes markers in sourcecode and builds ranges from them
    /// ranges are used to determine which line corresponds to which file
    static void buildRanges(std::u8string& sourceCode);

    /// @brief adds marker to array to match against
    static void depthMapping(std::u8string fileName);

    /// @brief immidiatly dumps error to console, no line input
    static void dumpAndBuildError(std::u8string text);

    /// @brief queues Error without line, gets dumped via dumpErrorLog() like usual
    static void queueUndefinedError(std::u8string name);
};
