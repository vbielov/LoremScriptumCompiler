#include "TestLexer.hpp"
#include "gtest/gtest.h"

TEST(BasicTest, TestLexerWithBasicCode) {
    std::vector<Token> token;
    std::stringstream oss;
    Lexer(SOURCE_CODE_TO_TEST).tokenize(token, oss);

    for (std::vector<Token>::size_type i = 0; i < token.size() && i < EXPECTED_TOKENS.size(); i++)
    {
        auto expectedToken = EXPECTED_TOKENS[i];
        auto actualToken = token[i];
        EXPECT_EQ(actualToken.type, expectedToken.type);
        EXPECT_STREQ((const char*)(actualToken.value.c_str()), (const char*)(expectedToken.value.c_str()));
        
    }
    // for(const auto& actualToken : token) {
    //     std::cout << "{TokenType::" << TOKEN_TYPE_LABELS[(int)actualToken.type] << ", u8\"" << (const char*)(actualToken.value.c_str()) << "\"}," << std::endl;
    // }

    EXPECT_EQ(token.size(), EXPECTED_TOKENS.size());
}