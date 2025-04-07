#include "ErrorHandler.hpp"

ErrorHandler *ErrorHandler::getInstance()
{
    return nullptr;
}

void ErrorHandler::init(std::u8string &sourceCode, std::u8string fileName, std::u8string fileLocation, bool instantDump)
{
}

void ErrorHandler::setInstantDump(bool state)
{
}

bool ErrorHandler::hasError()
{
    return false;
}

bool ErrorHandler::hasWarning()
{
    return false;
}

void ErrorHandler::grabSource(std::u8string sourceCode, std::string fileLocation)
{
}

void ErrorHandler::logError(size_t line, std::u8string reason)
{
}

void ErrorHandler::logWarning(size_t line, std::u8string reason)
{
}

void ErrorHandler::dumpErrorLog()
{
}

void ErrorHandler::buildRanges(std::u8string &sourceCode)
{
}

void ErrorHandler::depthMapping(std::u8string fileName)
{
}

void ErrorHandler::dumpAndBuildError(std::u8string text)
{
}

void ErrorHandler::queueUndefinedError(std::u8string name)
{
}
