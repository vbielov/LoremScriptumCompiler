#pragma once
#include "Parser.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class TestParserValid : public ::testing::TestWithParam<std::pair<std::u8string, std::string>> {};
class TestParserInvalid : public ::testing::TestWithParam<std::u8string> {};

std::string runParser(std::u8string& input);
std::string runParserInvalid(std::u8string& input);


TEST_P(TestParserValid, TestParserValid) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(expected, runParser(input)) << "Failed on input: " << reinterpret_cast<const char*>(input.c_str());
}
TEST_P(TestParserInvalid, TestParserInvalid) {
    auto input = GetParam();
    EXPECT_EQ("INVALID", runParserInvalid(input)) << "Failed on input: " << reinterpret_cast<const char*>(input.c_str());
}


// Debugging inspection zone

class TestParserDebugValid : public ::testing::TestWithParam<std::pair<std::u8string, std::string>> {};
class TestParserDebugInvalid : public ::testing::TestWithParam<std::u8string> {};

TEST_P(TestParserDebugValid, TestParserDebugValid) {
    auto [input, expected] = GetParam();
    EXPECT_EQ(expected, runParser(input)) << "Failed on input: " << reinterpret_cast<const char*>(input.c_str());
}
TEST_P(TestParserDebugInvalid, TestParserDebugInvalid) {
    auto input = GetParam();
    EXPECT_EQ("INVALID", runParserInvalid(input)) << "Failed on input: " << reinterpret_cast<const char*>(input.c_str());
}


INSTANTIATE_TEST_SUITE_P(TestParserDebugValid, TestParserDebugValid, ::testing::Values(
    std::make_pair(
        u8"nihil foo = λ(): ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       └── BlockAST\n"
    )
));

INSTANTIATE_TEST_SUITE_P(TestParserDebugInvalid, TestParserDebugInvalid, ::testing::Values(
    u8"nihil foo = λ(numerus id,): ;"
));