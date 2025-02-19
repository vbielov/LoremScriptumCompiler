#include "RomanNumber.hpp"
#include <stdexcept>

std::string toRomanConverter(int number){
    if(number == 0){ 
        return "O";
    };

    std::string roman = "";

    if(number < 0){
        roman.append("-");
        number = -number;
    }

    if (number > 3999){
        roman.append("inf"); //TODO: change to unicode thingy
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
int toArabicConverterAuto(std::string romanNumber){
    int arabic = 0;

    if(romanNumber == "O"){
        return arabic;
    }

    bool negative = false;
    if(romanNumber[0] == '-'){
        negative = true;
        romanNumber.erase(0,1);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'M'){
            arabic += 1000;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == 'C' && romanNumber[1] == 'M'){
        arabic += 900;
        romanNumber.erase(0,2);
    }


    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'D'){
            arabic += 500;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == 'C' && romanNumber[1] == 'D'){
        arabic += 400;
        romanNumber.erase(0,2);
    }


    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'C'){
            arabic += 100;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == 'X' && romanNumber[1] == 'C'){
        arabic += 90;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'L'){
            arabic += 50;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == 'X' && romanNumber[1] == 'L'){
        arabic += 40;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'X'){
            arabic += 10;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == 'I' && romanNumber[1] == 'X'){
        arabic += 9;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'V'){
            arabic += 5;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber.size() > 1 && romanNumber[0] == 'I' && romanNumber[1] == 'V'){
        arabic += 4;
        romanNumber.erase(0,2);
    }

    for(int i = 0; i < 4; i++){
        if(romanNumber.size() > 0 && romanNumber[0] == 'I'){
            arabic += 1;
            romanNumber.erase(0,1);
        } else {
            break;
        }
    }

    if(romanNumber != ""){
        throw std::invalid_argument("Roman Numeral does not follow rules!");
        return 0; 
    }

    if(negative){
        arabic = -arabic;
    }

    return arabic;
}


//faster for longer roman numerals
int toArabicConverter(std::string romanNumber){     // infinity is illegal
    int arabic = 0;
    
    if(romanNumber == "O"){
        return arabic;
    }

    bool negative = false;
    if(romanNumber[0] == '-'){
        negative = true;
        romanNumber.erase(0,1);
    }

    arabic += subConverter(romanNumber, Units);
    arabic += subConverter(romanNumber, Tens);
    arabic += subConverter(romanNumber, Hundreds);

    switch (romanNumber.size()){
        case 3:
            if(romanNumber == "MMM"){
                arabic += 3000;
                romanNumber = "";
            }
            break;
        case 2:
            if(romanNumber == "MM"){
                arabic += 2000;
                romanNumber = "";
            }
            break;
        case 1:
            if(romanNumber == "M"){
                arabic += 1000;
                romanNumber = "";
            }
            break;
        
    };

    if (romanNumber != ""){
        throw std::invalid_argument("Roman Numeral does not follow rules!");
        return 0;
    }

    if(negative){
        arabic = -arabic;
    }

    return arabic;
}; 


int subConverter(std::string& romanNumber, const ConvertData steps[]){
    std::string compare;
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
