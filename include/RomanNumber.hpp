#pragma once
#include <string>

struct ConvertData{
    std::u8string romanNumeral;
    int arabicNumeral;
};

inline static const ConvertData Units[] = {
    {u8"VIII", 8},
    {u8"VII", 7},
    {u8"III", 3},
    {u8"IX", 9},
    {u8"VI", 6},
    {u8"IV", 4},
    {u8"II", 2},
    {u8"V", 5},
    {u8"I", 1}
};

inline static const ConvertData Tens[] = {
    {u8"LXXX", 80},
    {u8"LXX", 70},
    {u8"XXX", 30},
    {u8"XC", 90},
    {u8"LX", 60},
    {u8"XL", 40},
    {u8"XX", 20},
    {u8"L", 50},
    {u8"X", 10}
};


inline static const ConvertData Hundreds[] = {
    {u8"DCCC", 800},
    {u8"DCC", 700},
    {u8"CCC", 300},
    {u8"CM", 900},
    {u8"DC", 600},
    {u8"CD", 400},
    {u8"CC", 200},
    {u8"D", 500},
    {u8"C", 100}
};

inline static const std::u8string OrderedUnits[] = {
    u8"I", u8"II", u8"III", u8"IV", u8"V", u8"VI", u8"VII", u8"VIII", u8"IX"
};

inline static const std::u8string OrderedTens[] = {
    u8"X", u8"XX", u8"XXX", u8"XL", u8"L", u8"LX", u8"LXX", u8"LXXX", u8"XC"
};


inline static const std::u8string OrderedHundreds[] = {
    u8"C", u8"CC", u8"CCC", u8"CD", u8"D", u8"DC", u8"DCC", u8"DCCC", u8"CM"
};

inline static const std::u8string OrderedThousands[] = {
    u8"M", u8"MM", u8"MMM"
};



std::u8string toRomanConverter(int number);

int toArabicConverterAuto(std::u8string romanNumber);

int toArabicConverter(std::u8string romanNumber); 

int subConverter(std::u8string& romanNumber, const ConvertData steps[]);

