#pragma once
#include "Parser.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

std::string runParser(std::u8string& input) {
    Lexer lexer(input);
    Parser parser(lexer, true);
    auto block = parser.parseBlock();

    std::ostringstream oss;
    if (block != nullptr) {
        block->printTree(oss, "", false);
    } else oss << "block is nullptr";

    return oss.str();
}

std::string runParserInvalid(std::u8string& input) {
    Lexer lexer(input);
    Parser parser(lexer, true);
    auto block = parser.parseBlock();

    std::ostringstream oss;
    if (block != nullptr) {
        block->printTree(oss, "", false);
    } else oss << "block is nullptr";

    return parser.isValid() ? oss.str() : "INVALID";
}

class ParserTestValid : public ::testing::TestWithParam<std::pair<std::u8string, std::string>> {};
class ParserTestInvalid : public ::testing::TestWithParam<std::u8string> {};

TEST_P(ParserTestValid, TestParserValid) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(expected, runParser(input)) << "Failed on input: " << reinterpret_cast<const char*>(input.c_str());
}

TEST_P(ParserTestInvalid, TestParserInvalid) {
    auto input = GetParam();
    EXPECT_EQ("INVALID", runParserInvalid(input)) << "Failed on input: " << reinterpret_cast<const char*>(input.c_str());
}
