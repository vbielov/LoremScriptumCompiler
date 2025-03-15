#pragma once
#include <string>
#include <regex>
#include <vector>
#include <filesystem>
#include <unordered_set>

//temp import
#include <iostream>

// returns bool based on if any errors happened before hand
bool error();

// stores pointer of source code
void grabSource(std::u8string sourceCode, std::string fileLocation);

// check previous and next token and gather what line its on
void parseForErrorType();

// build string with expected token
// get line from source file to print aswell
void buildString(size_t line, std::u8string reason);


// dumps all ErrorLogs into terminal and
void dumpErrorLog();


// did you mean? for typos
void typoDetection();

//
void setFile(std::u8string fileName, std::u8string code, size_t pos, bool noPos);

// struct fileLength{
//     std::u8string fileName;
//     size_t length;
//     size_t pos;
//     bool noPos;
//     bool body;
// };

struct fileLength{
    std::u8string fileName;
    size_t lines;
    size_t lineTillPos;
    size_t pos;
    bool noPos;
};

//
void buildRanges(std::u8string& sourceCode);

struct fileRange{
    std::u8string fileName;
    size_t start;
    size_t end;
};



void depthMapping(std::u8string fileName);