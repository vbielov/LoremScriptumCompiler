#include "ErrorHandler.hpp"
#include "RomanNumber.hpp"



static std::u8string output = u8"----------------------- Error encountered in ";
static bool anyErrors = false;

static std::vector<std::u8string> sourceArray;
static std::vector<fileLength> fileIndexes; // first entry is body
static std::vector<fileRange> fileRanges;

static std::vector<std::u8string> depth;

static std::u8string file;

size_t currentLine = 0;



struct rangeResult {
    std::u8string fileName;
    size_t displayline;
};

rangeResult getFileName(size_t line){

    static std::vector<fileRange> temp;

    rangeResult closestMatch;
    size_t distance ;
    size_t meantimeDist=0;
    bool firstMatch = true;

    for (size_t i = 0; i < fileRanges.size(); i++){
        size_t start = fileRanges[i].start;
        size_t end = fileRanges[i].end;


        if(start <= line){
        size_t displayline = line - start; //nonfunctional
            
        if(line == end){
            distance = line-meantimeDist; //... difficult

            if(firstMatch){
                distance = displayline;
            }

            closestMatch.displayline = distance+1;
            closestMatch.fileName = fileRanges[i].fileName;
            break;
        }
        
        if(line < end){
            

            if(firstMatch){
                distance = displayline;
                firstMatch = false;
            }

            closestMatch.displayline = distance+1;
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


void setFile(std::u8string fileName, std::u8string code, size_t pos, bool noPos){
    fileLength file(fileName, getLineCount(code), getLineCountTilPos(code, pos), pos, noPos);
    fileIndexes.push_back(file);


    // std::cout << (const char*) code.c_str() << std::endl;
    // std::cout << (const char*) file.fileName.c_str() << " length: " << file.lines << " pos: " << pos << " " << noPos << " " << file.lines << std::endl;

};


// void setFile(std::u8string fileName, size_t length, size_t pos, bool noPos, bool body){
    // fileLength file(fileName, length, pos, noPos, body);
    // fileIndexes.push_back(file);

    // std::cout << (const char*) file.fileName.c_str() << " length: " << length << " pos: " << pos << " " << noPos << " " << body << std::endl;


    //fileLength file(fileName, );
//};

// static size_t currentPosition = 1;
// void buildRanges(){
//     //TODO: implement


//     // if(pos == 0){

//     // }

//     for (int i = 0; i < fileIndexes.size(); i++){
//         if(fileIndexes[i].noPos){
//             fileRange file (fileIndexes[i].fileName, currentPosition, fileIndexes[i].lines);
//             currentPosition += fileIndexes[i].lines;
//         } else {
//             fileRange file (fileIndexes[i].fileName, currentPosition, fileIndexes[i].lines);

//         }

//         std::cout << (const char*) fileIndexes[i].fileName.c_str() << std::endl;
//     }


// };



fileRange fileRangeBuilder(std::u8string fileName, size_t start, size_t end) {
    fileRange newRange;
    newRange.fileName = fileName;
    newRange.start = start;
    newRange.end = end;
    return newRange;
}

void buildRanges(std::u8string& sourceCode){

    static std::vector<fileRange> temp;

    for(size_t i = 0; i < depth.size(); i++){
        std::string depthVals = std::to_string(i);
        std::u8string depthCount(depthVals.begin(), depthVals.end());
        std::u8string depthStart = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH" + depthCount;
        std::u8string depthEnd = u8".-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-. DEPTH end" + depthCount;

        size_t pos = sourceCode.find(depthStart); 
        size_t pos2 = sourceCode.find(depthEnd); 


        if (pos != std::u8string::npos && pos != std::u8string::npos) {
            fileRange range(depth[i], getLineCountTilPos(sourceCode, pos), getLineCountTilPos(sourceCode, pos2));
            temp.push_back(range);

            // std::cout << (const char*)range.fileName.c_str() << " start line: " << std::to_string(range.start) << " end line: " << std::to_string(range.end) << std::endl;

            sourceCode.erase(pos2, depthEnd.length());  
            sourceCode.erase(pos, depthStart.length());          
        }

    }

    std::sort(temp.begin(), temp.end(), [](const fileRange& a, const fileRange& b) {
        return a.start < b.start;
    });

    for(int i = 0; i < temp.size(); i++){
        std::cout << (const char*)temp[i].fileName.c_str() << " start line: " << std::to_string(temp[i].start) << " end line: " << std::to_string(temp[i].end) << std::endl;
    }


    fileRanges = temp;

    for(int i=1; i < 50; i++ ){
        std::cout<< "real pos "<< std::to_string(i) << " relative pos " << std::to_string(getFileName(i).displayline) << " filename: " << (const char*) getFileName(i).fileName.c_str() << std::endl;
    }
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
    build.append( u8"\x1b]8;;vscode://file/"+ stringAbsPath + u8":" + uLine2 + u8"\x1b\\"+ uLine2 + u8"\x1b]8;;\x1b\\");

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