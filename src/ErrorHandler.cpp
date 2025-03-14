#include "ErrorHandler.hpp"
#include "RomanNumber.hpp"



static std::u8string output = u8"----------------------- Error encountered in ";
static bool anyErrors = false;

static std::vector<std::u8string> sourceArray;
static std::vector<fileLength> fileIndexes;
static std::u8string file;

void setFile(std::u8string fileName, size_t length, size_t pos, bool noPos, bool body){
    fileLength file(fileName, length, pos, noPos, body);
    fileIndexes.push_back(file);

    std::cout << (const char*) file.fileName.c_str() << " length: " << length << " pos: " << pos << noPos << body << std::endl;
};

void buildRanges(std::vector<fileLength> fileIndexes){
    //TODO: implement
};


void grabSource(std::u8string sourceCode, std::string fileLocation){
    std::u8string line = u8"";

    bool isNotIndent = false;
    char letter;
    for (size_t i = 0; i < sourceCode.length(); i++){
        letter = sourceCode.at(i);

        if(letter == '\n'){
            sourceArray.push_back(line);
            line = u8"";
            isNotIndent = false;
        } else {
            if(isNotIndent || letter != ' '){
                isNotIndent = true;
                line += letter;
            }            
        }
    }

    sourceArray.push_back(line);

    std::u8string temp(fileLocation.begin(), fileLocation.end()); //TODO DELETE
    file = temp;
    output.append(temp);
    output.append(u8" -----------------------");

    output.append(u8"\n bracket errors can be very inconsistant with more errors mixed in, for ease of use consider using our vscode extention for highlighting\n\n");
}

bool error(){
    return anyErrors;
}


void buildString(size_t line, std::u8string reason){ // add string parameter
    anyErrors = true;
    std::u8string build = u8"\n \033[1;41mError\033[0m encountered in Line ";
    
    
    // std::string sLine = std::to_string(line);
    //std::u8string uLine(sLine.begin(), sLine.end());


    std::filesystem::path relativePath = (const char*)file.c_str();
    std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);
    
    std::u8string stringAbsPath=     absolutePath.generic_u8string();

    std::u8string uLine = toRomanConverter((int) line);

    std::string sLine = std::to_string(line);
    std::u8string uLine2(sLine.begin(), sLine.end());

    //TODO: try hide link => move to appropriate location
    build.append( u8"\x1b]8;;vscode://file/"+ stringAbsPath + u8":" + uLine2 + u8"\x1b\\"+ uLine + u8"\x1b]8;;\x1b\\");

    //build += uLine;
    build.append(u8"\n        \033[31m");

    // append line from
    if(sourceArray.size() >= line-1){
        build.append(sourceArray[line-1]);
    } else {
        build.append(u8"failure to obtain line");
    }

    build.append(u8"\033[0m \n possible Reason: ");
    build.append(reason);
    // append token


    build.append(u8"\n");
    output.append(build);

}

void dumpErrorLog(){
    std::cout << (const char*) output.c_str() << std::endl;
}