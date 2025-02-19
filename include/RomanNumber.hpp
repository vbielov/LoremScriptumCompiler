#pragma once
#include <string>



struct ConvertData{
    std::string romanNumeral;
    int arabicNumeral;
};

inline static const ConvertData Units[] = {
    {"VIII", 8},
    {"VII", 7},
    {"III", 3},
    {"IX", 9},
    {"VI", 6},
    {"IV", 4},
    {"II", 2},
    {"V", 5},
    {"I", 1}
};

inline static const ConvertData Tens[] = {
    {"LXXX", 80},
    {"LXX", 70},
    {"XXX", 30},
    {"XC", 90},
    {"LX", 60},
    {"XL", 40},
    {"XX", 20},
    {"L", 50},
    {"X", 10}
};


inline static const ConvertData Hundreds[] = {
    {"DCCC", 800},
    {"DCC", 700},
    {"CCC", 300},
    {"CM", 900},
    {"DC", 600},
    {"CD", 400},
    {"CC", 200},
    {"D", 500},
    {"C", 100}
};

inline static const std::string OrderedUnits[] = {
    "I", "II", "III","IV", "V", "VI", "VII", "VIII", "IX"
};

inline static const std::string OrderedTens[] = {
    "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"
};


inline static const std::string OrderedHundreds[] = {
    "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"
};

inline static const std::string OrderedThousands[] = {
    "M", "MM", "MMM"
};



std::string toRomanConverter(int number);

int toArabicConverterAuto(std::string romanNumber);

int toArabicConverter(std::string romanNumber); 

int subConverter(std::string& romanNumber, const ConvertData steps[]);

