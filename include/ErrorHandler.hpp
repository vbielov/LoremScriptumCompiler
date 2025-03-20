#pragma once
#include <string>
#include <regex>
#include <vector>
#include <filesystem>
#include <unordered_set>
#include <iostream>
#include "RomanNumber.hpp"

struct fileLength{
    std::u8string fileName;
    size_t lines;
    size_t lineTillPos;
    size_t pos;
};

struct fileRange{
    std::u8string fileName;
    size_t start;
    size_t end;
};


struct rangeResult {
    std::u8string fileName;
    size_t displayline;
};



// returns bool based on if any errors happened before hand
bool error();

// splits source into individual lines
void grabSource(std::u8string sourceCode, std::string fileLocation);


// build string with reason as error message
// get line from source file to print aswell
// printed via dumpErrorLog
void buildString(size_t line, std::u8string reason);


// dumps all ErrorLogs into terminal
void dumpErrorLog();


// removes markers in sourcecode and builds ranges from them
// ranges are used to determine which line corresponds to which file
void buildRanges(std::u8string& sourceCode);

// adds marker to array to match against
void depthMapping(std::u8string fileName);

// immidiatly dumps error to console, no line input
void dumpAndBuildError(std::u8string text);

// queues Error without line, gets dumped via dumpErrorLog() like usual
void queueUndefinedError(std::u8string name);



// canceled

// did you mean? for typos
void typoDetection();