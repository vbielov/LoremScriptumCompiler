#pragma once
#include <string>
#include <regex>
#include <vector>
#include <filesystem>

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