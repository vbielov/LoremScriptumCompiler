#include "RomanNumber.hpp"
#include <stdexcept>

std::u8string toRomanConverter(int number){
    if(number == 0){ 
        return u8"O";
    };

    std::u8string roman = u8"";

    if(number < 0){
        roman.append(u8"-");
        number = -number;
    }

    if (number > 3999){
        roman.append(u8"∞"); //TODO: test if functional
        return roman;
    };


    int temp;
    if (number > 999){
        temp = number/1000 - 1;
        number = number%1000;
        roman.append(OrderedThousands[temp]);
    };

    if (number > 99){
        temp = number/100 - 1;
        number = number%100;
        roman.append(OrderedHundreds[temp]);
    };

    if (number > 9){
        temp = number/10 - 1;
        number = number%10;
        roman.append(OrderedTens[temp]);
    };


    if (number > 0){
        roman.append(OrderedUnits[number-1]);
    };

    return roman;
};


// quickest on average
bool toArabicConverterAuto(std::u8string romanNumber, int* outArabic){
    if(outArabic == nullptr){
        return false;
    }
    
    *outArabic = 0;

    if(romanNumber == u8"O"){
        return true;
    }

    bool negative = false;
    if(romanNumber[0] == u8'-'){
        negative = true;
        romanNumber.erase(0,1);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'M'){
            *outArabic += 1000;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == u8'C' && romanNumber[1] == u8'M'){
        *outArabic += 900;
        romanNumber.erase(0,2);
    }


    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'D'){
            *outArabic += 500;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == u8'C' && romanNumber[1] == u8'D'){
        *outArabic += 400;
        romanNumber.erase(0,2);
    }


    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'C'){
            *outArabic += 100;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == u8'X' && romanNumber[1] == u8'C'){
        *outArabic += 90;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'L'){
            *outArabic += 50;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == u8'X' && romanNumber[1] == u8'L'){
        *outArabic += 40;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'X'){
            *outArabic += 10;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == u8'I' && romanNumber[1] == u8'X'){
        *outArabic += 9;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'V'){
            *outArabic += 5;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == u8'I' && romanNumber[1] == u8'V'){
        *outArabic += 4;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == u8'I'){
            *outArabic += 1;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber != u8""){ //illegal string was input
        return false; 
    }

    if(negative){
        *outArabic = -*outArabic;
    }


    return true;
}


//faster for longer roman numerals
bool toArabicConverter(std::u8string romanNumber, int* outArabic){     // infinity is illegal
    if(outArabic == nullptr){
        return false;
    }
    
    *outArabic = 0;
    
    if(romanNumber == u8"O"){
        return true;
    }

    bool negative = false;
    if(romanNumber[0] == u8'-'){
        negative = true;
        romanNumber.erase(0,1);
    }

    *outArabic += subConverter(romanNumber, Units);
    *outArabic += subConverter(romanNumber, Tens);
    *outArabic += subConverter(romanNumber, Hundreds);

    switch (romanNumber.size()){
        case 3:
            if(romanNumber == u8"MMM"){
                *outArabic += 3000;
                romanNumber = u8"";
            }
            break;
        case 2:
            if(romanNumber == u8"MM"){
                *outArabic += 2000;
                romanNumber = u8"";
            }
            break;
        case 1:
            if(romanNumber == u8"M"){
                *outArabic += 1000;
                romanNumber = u8"";
            }
            break;
        
    };

    if (romanNumber != u8""){
        return false;
    }

    if(negative){
        *outArabic = -*outArabic;
    }

    return true;
}; 


int subConverter(std::u8string& romanNumber, const ConvertData steps[]){
    std::u8string compare;
    int stringSize = romanNumber.size();

    if (stringSize > 3){ // size 4 check
        
        compare = romanNumber.substr(stringSize-4, stringSize);

        ConvertData units = steps[0];
        if(units.romanNumeral == compare){
            romanNumber.erase(stringSize-4);
            return units.arabicNumeral;
        }
    };

    if (stringSize > 2){
        compare = romanNumber.substr(stringSize-3, stringSize);

        for(int i = 1; i < 3; i++){
            ConvertData units = steps[i];
            if(units.romanNumeral == compare){
                romanNumber.erase(stringSize-3);
                return units.arabicNumeral;
            }
        }
    };

    if (stringSize > 1){
        compare = romanNumber.substr(stringSize-2, stringSize);

        for(int i = 3; i < 7; i++){
            ConvertData units = steps[i];
            if(units.romanNumeral == compare){
                romanNumber.erase(stringSize-2);
                return units.arabicNumeral;
            }
        }

    };

    if (stringSize > 0){
        compare = romanNumber.substr(stringSize-1, stringSize);

        for(int i = 7; i < 9; i++){
            ConvertData units = steps[i];
            if(units.romanNumeral == compare){
                romanNumber.erase(stringSize-1);
                return units.arabicNumeral;
            }
        }

    } 
    
    return 0;
}
