#include "RomanNumber.hpp"
#include "gtest/gtest.h"
#include <stdexcept>

TEST(BasicTest, MethodVsMethod){
    int autoArabic = 0;
    int arabic = 0;
    for (int i = -3999; i < 4000; i++){
        EXPECT_TRUE(toArabicConverter(toRomanConverter(i), &arabic));
        EXPECT_EQ(arabic, i);
        
        EXPECT_TRUE(toArabicConverterAuto(toRomanConverter(i), &autoArabic));
        EXPECT_EQ(autoArabic, i);
    }
}


TEST(BasicTest, outOfBounds){
    EXPECT_EQ(toRomanConverter(4000), u8"inf");
    EXPECT_EQ(toRomanConverter(-4000), u8"-inf");
}

TEST(BasicTest, failure){
    int arabic = 0;
    EXPECT_FALSE(toArabicConverter(u8"CMCM", &arabic));
    EXPECT_FALSE(toArabicConverterAuto(u8"CMCM", &arabic));
}

