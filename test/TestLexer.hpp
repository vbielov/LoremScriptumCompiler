#pragma once
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "Lexer.hpp"
#include <vector>

inline static const std::u8string SOURCE_CODE_TO_TEST = u8R"(
    numerus foo = λ(numerus a, numerus b):
        a = a + b
        b = a * II
        retro b
    ;
    numerus c = foo(I, V)

    numerus cadabra = XVI

    ∑(∞):
        numerus abra = II
        abra = abra + abra
        
        si abra > cadabra:
            finio
        ;    
    ;

    scribor(abra)

    literra test = '\n'

    scribor(test)

    scribor(abra ≠ cadabra)
    scribor(abra == cadabra)
    scribor(abra < cadabra)
    scribor(abra > cadabra)

    scribor(abra + cadabra)
    scribor(abra - cadabra)
    scribor(abra ^ cadabra)
    scribor(abra / cadabra)
    scribor(¬cadabra)
    scribor(abra!)

    // numerus addition = λ(numerus α, numerus beta): retro α + β;
)";

inline static const std::vector<Token> EXPECTED_TOKENS = {
    {TokenType::TYPE, u8"numerus"},
    {TokenType::IDENTIFIER, u8"foo"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::KEYWORD, u8"λ"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::TYPE, u8"numerus"},
    {TokenType::IDENTIFIER, u8"a"},
    {TokenType::PUNCTUATION, u8","},
    {TokenType::TYPE, u8"numerus"},
    {TokenType::IDENTIFIER, u8"b"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::PUNCTUATION, u8":"},
    {TokenType::IDENTIFIER, u8"a"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::IDENTIFIER, u8"a"},
    {TokenType::OPERATOR, u8"+"},
    {TokenType::IDENTIFIER, u8"b"},
    {TokenType::IDENTIFIER, u8"b"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::IDENTIFIER, u8"a"},
    {TokenType::OPERATOR, u8"*"},
    {TokenType::NUMBER, u8"II"},
    {TokenType::KEYWORD, u8"retro"},
    {TokenType::IDENTIFIER, u8"b"},
    {TokenType::PUNCTUATION, u8";"},
    {TokenType::TYPE, u8"numerus"},
    {TokenType::IDENTIFIER, u8"c"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::IDENTIFIER, u8"foo"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::NUMBER, u8"I"},
    {TokenType::PUNCTUATION, u8","},
    {TokenType::NUMBER, u8"V"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::TYPE, u8"numerus"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::NUMBER, u8"XVI"},
    {TokenType::KEYWORD, u8"∑(∞)"},
    {TokenType::PUNCTUATION, u8":"},
    {TokenType::TYPE, u8"numerus"},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::NUMBER, u8"II"},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"+"},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::KEYWORD, u8"si"},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8">"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8":"},
    {TokenType::KEYWORD, u8"finio"},
    {TokenType::PUNCTUATION, u8";"},
    {TokenType::PUNCTUATION, u8";"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::TYPE, u8"literra"},
    {TokenType::IDENTIFIER, u8"test"},
    {TokenType::OPERATOR, u8"="},
    {TokenType::LITERAL, u8"\\n"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"test"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"≠"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"=="},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"<"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8">"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"+"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"-"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"^"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"/"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::OPERATOR, u8"¬"},
    {TokenType::IDENTIFIER, u8"cadabra"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::IDENTIFIER, u8"scribor"},
    {TokenType::PUNCTUATION, u8"("},
    {TokenType::IDENTIFIER, u8"abra"},
    {TokenType::OPERATOR, u8"!"},
    {TokenType::PUNCTUATION, u8")"},
    {TokenType::COMMENT, u8"// numerus addition = λ(numerus α, numerus beta): retro α + β;"},
};

class TestLexer : public ::testing::Test {};

