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

            sourceCode.erase(pos2, depthEnd.length());  
            sourceCode.erase(pos, depthStart.length());          
        }

    }

    size_t rangeBegin = 0;

    // if(temp[0].end > temp[1].end){
    //     if(temp[0].start == temp[1].end){
    //         fileRanges.push_back(temp[1]); 

    //          if(temp[1].end > temp[2].end){

    //          }

    //          rangeBegin = temp[1].end; //temp 1 exits as it is completed
    //      };
    //  }

    size_t i = 0;
    static std::vector<fileRange> temp2;
    bool secondIteration = false;

    do{
        if(secondIteration){
            temp = temp2;
            temp2.clear();
        }
        while(1 < temp.size()){
            for(int i = 0; i < 2; i++){
                std::cout << (const char*) temp[i].fileName.c_str() << " start line " << std::to_string(temp[i].start) << " end line " << std::to_string(temp[i].end) << std::endl ; 
            
            }
            std::cout << std::endl;

            if(temp[i].end < temp[i+1].end || temp[i].start > temp[i+1].start){
                temp2.push_back(temp[i+1]);
                temp.erase(temp.begin()+i+1);
                if(temp.size() == 1){
                    fileRanges.push_back(temp[i]); 
                }
                continue;
            }

            if(temp[i].end > temp[i+1].end && temp[i].start < temp[i+1].start){ // somewhere in middle of program is import
                fileRanges.push_back(fileRangeBuilder(temp[i].fileName, temp[i].start, temp[i+1].end-1)); 
                // fileRanges.push_back(temp[i+1]);
                // i+1 is complete

                temp[i] = fileRangeBuilder(temp[i].fileName, temp[i+1].end+1, temp[i].end); //Set i to span rest 

                temp2.push_back(temp[i+1]);
                
                temp.erase(temp.begin()+i+1);


            }else if(temp[i].end==temp[i+1].end && temp[i].start == temp[i+1].start){ // entire programm is 1 import
                fileRanges.push_back(temp[i+1]); //TODO: FIX this is wonky
                //i and i+1 are complete
                //temp.erase(temp.begin()+ i);

                temp2.push_back(temp[i+1]);
                temp.erase(temp.begin()+i+1);

            }else if(temp[i].end==temp[i+1].end && temp[i].start > temp[i+1].start){ // import at end of programm
                //i and i+1 are complete
                //temp.erase(temp.begin()+i);

                temp2.push_back(temp[i+1]);
                temp.erase(temp.begin()+i+1);

            }else if(temp[i].end > temp[i+1].end && temp[i].start == temp[i+1].start){ // import at start of programm
                //fileRanges.push_back(temp[i+1]); 
                temp[i] = fileRangeBuilder(temp[i].fileName, temp[i+1].end+1, temp[i].end); //Set i to span rest 
                //i+1 is complete

                temp2.push_back(temp[i+1]);
                temp.erase(temp.begin()+i+1);
            }
        }
        secondIteration = true;

        if(temp.size() == 1){
            fileRanges.push_back(temp[i]); 
        }

    }while(!temp2.empty() && 1 < temp2.size());

    
    

    for(int i = 0; i < fileRanges.size(); i++){
        std::cout << (const char*) fileRanges[i].fileName.c_str() << " start line " << std::to_string(fileRanges[i].start) << " end line " << std::to_string(fileRanges[i].end) << std::endl ; 
    
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