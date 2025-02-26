#include "TestLexer.hpp"
#include "gtest/gtest.h"

TEST(BasicTest, TestLexerWithBasicCode) {
    Lexer lexer = Lexer(SOURCE_CODE_TO_TEST);

    for(const auto& expectedToken : EXPECTED_TOKENS) {
        Token testToken = lexer.getNextToken();
        // std::cout << "{TokenType::" << TOKEN_TYPE_LABELS[(int)testToken.type] << ", u8\"" << (const char*)(testToken.value.c_str()) << "\"}," << std::endl;
        EXPECT_EQ(testToken.type, expectedToken.type);
        EXPECT_STREQ((const char*)(testToken.value.c_str()), (const char*)(expectedToken.value.c_str()));
    }
}