#pragma once
#include <string>
#include <regex>
#include <vector>
#include <filesystem>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "RomanNumber.hpp"
#include <mutex>
#include <assert.h>
#include "SourceLine.hpp"

class ErrorHandler {
private:
    const std::vector<SourceLine>* m_sourceLines; // Reference to source lines for error reporting
    bool m_errorFlag;
    bool m_warnFlag;

    inline static ErrorHandler* s_instance = nullptr; // Singleton instance
    inline static std::mutex s_mutex; // Mutex for thread safety
    ErrorHandler() : m_sourceLines(nullptr), m_errorFlag(false), m_warnFlag(false) {} // Private constructor for singleton pattern

public:
    // Deleting copy constructor and assignment operator to prevent copying
    ErrorHandler(const ErrorHandler& obj) = delete;

    /// @brief Static method to get the singleton instance of ErrorHandler
    static ErrorHandler* getInstance();

    static void init(const std::vector<SourceLine>& lines);

    /// @return returns bool based on if any errors happened before hand
    static bool hasError();

    /// @return returns bool based on detected warnings
    static bool hasWarning();

    /// @brief build string with reason as error message
    /// get line from source file to print aswell
    /// printed via dumpErrorLog
    static void logError(std::u8string reason, size_t line);
    static void logError(std::u8string reason);

    /// @brief works like logError but doesn't stop program flow
    static void logWarning(std::u8string reason, size_t);
    static void logWarning(std::u8string reason);

private:
    void log(size_t* line, std::u8string reason, bool isError);
};