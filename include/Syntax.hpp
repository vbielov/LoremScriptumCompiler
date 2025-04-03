#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

namespace punctuation {
    inline constexpr std::u8string_view PAREN_OPEN = u8"(";
    inline constexpr std::u8string_view PAREN_CLOSE = u8")";
    inline constexpr std::u8string_view BLOCK_OPEN = u8":";
    inline constexpr std::u8string_view BLOCK_CLOSE = u8";";
    inline constexpr std::u8string_view COMMA = u8",";
    inline constexpr std::u8string_view SQR_BRACKET_OPEN = u8"[";
    inline constexpr std::u8string_view SQR_BRACKET_CLOSE = u8"]";
    inline constexpr std::u8string_view APOSTROPHE = u8"'";
    inline constexpr std::u8string_view QUOTE = u8"\"";

    inline constexpr std::u8string_view VALUES[] = {
        PAREN_OPEN, PAREN_CLOSE, BLOCK_OPEN, BLOCK_CLOSE,
        COMMA, SQR_BRACKET_OPEN, SQR_BRACKET_CLOSE, APOSTROPHE, QUOTE
    };
    inline constexpr size_t VALUES_SIZE = sizeof(VALUES) / sizeof(VALUES[0]); 
}

namespace keywords {
    inline constexpr std::u8string_view FUNCTION = u8"λ";
    inline constexpr std::u8string_view RETURN = u8"retro";
    inline constexpr std::u8string_view BREAK = u8"finio";
    inline constexpr std::u8string_view FOR_LOOP = u8"∑";
    inline constexpr std::u8string_view IF = u8"si";
    inline constexpr std::u8string_view ELIF = u8"nisi";
    inline constexpr std::u8string_view ELSE = u8"ni";
    inline constexpr std::u8string_view INCLUDE = u8"apere";

    inline constexpr std::u8string_view VALUES[] = {
        FUNCTION, RETURN, BREAK, FOR_LOOP,
        IF, ELIF, ELSE, INCLUDE
    };
    inline constexpr size_t VALUES_SIZE = sizeof(VALUES) / sizeof(VALUES[0]); 
}

namespace types {
    inline constexpr std::u8string_view INT = u8"numerus";
    inline constexpr std::u8string_view BOOL = u8"asertio";
    inline constexpr std::u8string_view CHAR = u8"litera";
    inline constexpr std::u8string_view VOID = u8"nihil";
    inline constexpr std::u8string_view STRUCT = u8"rerum";

    inline constexpr std::u8string_view VALUES[] = {
        INT, BOOL, CHAR, VOID, STRUCT
    };
    inline constexpr size_t VALUES_SIZE = sizeof(VALUES) / sizeof(VALUES[0]); 
}

namespace boolean_types {
    inline constexpr std::u8string_view TRUE = u8"veri";
    inline constexpr std::u8string_view FALSE = u8"falso";

    inline constexpr std::u8string_view VALUES[] = { TRUE, FALSE };
    inline constexpr size_t VALUES_SIZE = sizeof(VALUES) / sizeof(VALUES[0]); 
}

namespace operators {
    inline constexpr std::u8string_view ASSIGN = u8"=";
    inline constexpr std::u8string_view EQUAL = u8"⇔";
    inline constexpr std::u8string_view NOT_EQUAL = u8"≠";
    inline constexpr std::u8string_view GREATER = u8">";
    inline constexpr std::u8string_view LESSER = u8"<";
    inline constexpr std::u8string_view GREATER_OR_EQUAL = u8"≥";
    inline constexpr std::u8string_view LESSER_OR_EQUAL = u8"≤";
    inline constexpr std::u8string_view PLUS = u8"+";
    inline constexpr std::u8string_view MINUS = u8"-";
    inline constexpr std::u8string_view MULTIPLY = u8"×";
    inline constexpr std::u8string_view DIVIDE = u8"÷";
    inline constexpr std::u8string_view MODULO = u8"%";
    inline constexpr std::u8string_view POWER = u8"^";
    inline constexpr std::u8string_view NOT = u8"¬";
    inline constexpr std::u8string_view FACTORIAL = u8"!";
    inline constexpr std::u8string_view AND = u8"∧";
    inline constexpr std::u8string_view OR = u8"∨";

    inline constexpr std::u8string_view VALUES[] = {
        ASSIGN, EQUAL, NOT_EQUAL, GREATER, LESSER,
        GREATER_OR_EQUAL, LESSER_OR_EQUAL, PLUS, MINUS,
        MULTIPLY, DIVIDE, MODULO, POWER, NOT,
        FACTORIAL, AND, OR
    };
    inline constexpr size_t VALUES_SIZE = sizeof(VALUES) / sizeof(VALUES[0]); 

    // source: https://en.wikipedia.org/wiki/Order_of_operations
    // smaller number means higher priority
    inline const std::unordered_map<std::u8string_view, int> BINARY_OPERATION_PRIORITY_MAP = {
        {FACTORIAL, 2},
        {NOT, 2},
        {POWER, 2},

        {MULTIPLY, 3},
        {DIVIDE, 3},
        {MODULO, 3},

        {PLUS, 4},
        {MINUS, 4},

        {LESSER, 6},
        {GREATER_OR_EQUAL, 6},
        {LESSER_OR_EQUAL, 6},
        {GREATER, 6},

        {EQUAL, 7},
        {NOT_EQUAL, 7},

        {AND, 11},

        {OR, 12},

        {ASSIGN, 14},
    };
}
