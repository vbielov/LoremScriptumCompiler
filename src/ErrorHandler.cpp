#include "ErrorHandler.hpp"

static std::u8string output = u8"----------------------- Error/s or Warning/s while compiling  "; // stores log
static std::u8string warningLogs = u8"";

static std::vector<std::u8string> sourceArray; // vector containing lines
static std::vector<fileRange> fileRanges;
static std::vector<std::u8string> depth;

std::u8string file; // filename that gets compiled

static bool anyErrors = false;
bool anyWarnings = false;
bool instantDump = false;


// ---------- helper methods/procedures ---------- 

size_t calcDistance(size_t line, size_t start){ // iterates over file ranges to calculate line of the correct source file
    size_t position = start;
    size_t distance = 1;
    for (size_t j = 0; j < fileRanges.size()-1; j++){
        if(fileRanges[j].start > position && position < line && fileRanges[j].start < line){
            distance += fileRanges[j].start - position;
            position = fileRanges[j].end;
        }        
    }

    distance += line-position;

    return distance;
}




rangeResult getFileName(size_t line){
    rangeResult closestMatch;

    for (size_t i = 0; i < fileRanges.size(); i++){ // finds correct sourcefile
        size_t start = fileRanges[i].start;
        size_t end = fileRanges[i].end;


        if(start <= line){
            
            if(line == end){
                closestMatch.displayline = calcDistance(line, start);
                closestMatch.fileName = fileRanges[i].fileName;
                break;
            }
            
            if(line < end){
                closestMatch.displayline = calcDistance(line, start);
                closestMatch.fileName = fileRanges[i].fileName;
            }
        }
    }


    return closestMatch;
    
}

size_t getLineCount(std::u8string text){
    size_t line = 1;
    for (size_t i = 0; i < text.length(); i++){
        if(text.at(i) == '\n'){
            line++;
        }
    }
    return line;
}

size_t getLineCountTilPos(std::u8string text, size_t pos){
    size_t line = 1;
    for (size_t i = 0; i <= pos; i++){
        if(text.at(i) == '\n'){
            line++;
        }
    }
    return line;
}

// ---------- end of helper methods ---------- 

void buildRanges(std::u8string& sourceCode){
    for(size_t i = 0; i < depth.size(); i++){
        std::string depthVals = std::to_string(i);
        std::u8string depthCount(depthVals.begin(), depthVals.end());
        std::u8string depthStart = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH" + depthCount;
        std::u8string depthEnd = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH end" + depthCount;

        size_t pos = sourceCode.find(depthStart); 
        size_t pos2 = sourceCode.find(depthEnd); 


        if (pos != std::u8string::npos && pos != std::u8string::npos) {
            fileRange range(depth[i], getLineCountTilPos(sourceCode, pos), getLineCountTilPos(sourceCode, pos2));
            fileRanges.push_back(range);


            sourceCode.erase(pos2, depthEnd.length());  
            sourceCode.erase(pos, depthStart.length());          
        }

    }

    std::sort(fileRanges.begin(), fileRanges.end(), [](const fileRange& a, const fileRange& b) { // sort required so that file origin algorithm works
        return a.start < b.start;
    });
}


void depthMapping(std::u8string fileName){
    depth.push_back(fileName);
}


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

    std::u8string temp(fileLocation.begin(), fileLocation.end()); 
    output.append(temp);
    output.append(u8" -----------------------");

    output.append(u8"\n bracket errors can be very inconsistant with more errors mixed in, for ease of use consider using our vscode extention for highlighting\n\n");

    if(instantDump){
        std::cout << (const char*) output.c_str() << std::endl;
    }
}

bool error(){
    return anyErrors;
}

bool warn(){
    return anyWarnings;
}


void logError(size_t line, std::u8string reason){ 
    anyErrors = true;
    std::u8string build = u8"\n \033[1;41mError\033[0m encountered in Line ";
    

    rangeResult data = getFileName(line);
    
    std::u8string stringAbsPath = data.fileName;

    std::u8string uLine = toRomanConverter(data.displayline);

    std::string sLine = std::to_string(data.displayline);
    std::u8string uLine2(sLine.begin(), sLine.end());


    build.append( u8"\x1b]8;;vscode://file/"+ stringAbsPath + u8":" + uLine2 + u8"\x1b\\"+ uLine + u8"\x1b]8;;\x1b\\" + u8" in File: "+ stringAbsPath);

    //build += uLine;
    build.append(u8"\n        \033[31m");

    // append line from
    if(sourceArray.size() > line-1){
        build.append(sourceArray[line-1]);
    } else {
        build.append(u8"failure to obtain line");
    }

    build.append(u8"\033[0m \n possible Reason: ");
    build.append(reason);
    // append token


    build.append(u8"\n");

    if(instantDump){
        std::cout << (const char*) build.c_str() << std::endl;
    }else{
        output.append(build);
    }
}

void logWarning(size_t line, std::u8string reason){
    anyWarnings = true;
    std::u8string build = u8"\n \033[1;48;5;214mWarning\033[0m for Line ";
    

    rangeResult data = getFileName(line);

    
    std::u8string stringAbsPath = data.fileName;

    std::u8string uLine = toRomanConverter(data.displayline);

    std::string sLine = std::to_string(data.displayline);
    std::u8string uLine2(sLine.begin(), sLine.end());


    build.append( u8"\x1b]8;;vscode://file/"+ stringAbsPath + u8":" + uLine2 + u8"\x1b\\"+ uLine + u8"\x1b]8;;\x1b\\" + u8" in File: "+ stringAbsPath);

    //build += uLine;
    build.append(u8"\n        \033[38;5;214m");

    // append line from
    if(sourceArray.size() > line-1){
        build.append(sourceArray[line-1]);
    } else {
        build.append(u8"failure to obtain line");
    }

    build.append(u8"\033[0m \n Warning: ");
    build.append(reason);
    // append token


    build.append(u8"\n");

    if(instantDump){
        std::cout << (const char*) build.c_str() << std::endl;
    }else{
        warningLogs.append(build);
    }
}



void dumpErrorLog(){
    //insert warnings to end

    if(!instantDump){
        output.append(warningLogs);
        std::cout << (const char*) output.c_str() << std::endl;
    }
   
}




void dumpAndBuildError(std::u8string text){
    anyErrors = true;

    std::u8string output;
    output.append(u8"\n \033[1;41mError\033[0m ");
    output.append(text);
    output.append(u8"\n\n\n");
    std::cout << (const char*) output.c_str() << std::endl;
}


void queueUndefinedError(std::u8string text){
    anyErrors = true;
    output.append(u8"\n \033[1;41mError\033[0m ");
    output.append(text);
    output.append(u8"\n\n\n");
}


void setInstantDump(){
    instantDump = true;
}