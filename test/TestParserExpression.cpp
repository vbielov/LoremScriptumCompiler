#include "TestParser.hpp"

INSTANTIATE_TEST_SUITE_P(TestParserExpressionValid, ParserTestValid, ::testing::Values(
    std::make_pair(
        u8"I",
        "├── BlockAST\n"
        "│   └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"(I)",
        "├── BlockAST\n"
        "│   └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"(((I)))",
        "├── BlockAST\n"
        "│   └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"func()",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"((func()))",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"foo(I)",
        "├── BlockAST\n"
        "│   └── FuncCallAST(foo)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"foo(I, I)",
        "├── BlockAST\n"
        "│   └── FuncCallAST(foo)\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"foo(I, I, I)",
        "├── BlockAST\n"
        "│   └── FuncCallAST(foo)\n"
        "│       ├── NumberAST(1)\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"foo((func() + I), X == X-III, (V))",
        "├── BlockAST\n"
        "│   └── FuncCallAST(foo)\n"
        "│       ├── BinaryOperatorAST('+')\n"
        "│       │   ├── FuncCallAST(func)\n"
        "│       │   └── NumberAST(1)\n"
        "│       ├── BinaryOperatorAST('==')\n"
        "│       │   ├── NumberAST(10)\n"
        "│       │   └── BinaryOperatorAST('-')\n"
        "│       │       ├── NumberAST(10)\n"
        "│       │       └── NumberAST(3)\n"
        "│       └── NumberAST(5)\n"
    ),
    std::make_pair(
        u8"I + I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('+')\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"I - I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('-')\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"I * I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('*')\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"I / I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('/')\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I + I * III",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── BinaryOperatorAST('+')\n"
        "│           ├── NumberAST(1)\n"
        "│           └── BinaryOperatorAST('*')\n"
        "│               ├── NumberAST(1)\n"
        "│               └── NumberAST(3)\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserExpressionInvalid, ParserTestInvalid, ::testing::Values(
    u8"func(I,)",
    u8"()",
    u8"I +",
    u8"I + (I",
    u8"I + I + ",
    u8"I + (I + I",
    u8"I + (I + I) +",
    u8"I + (I +",
    u8"print(",
    u8"finio + III",
    u8"si - func()",
    u8"si: ;",
    u8"si I == I: ; nisi: ;",
    u8"si I == I: ; ni I == I: ;",
    u8"si I == I: ; ni I == I: si I == I: ;",
    u8"nisi - X",
    u8"X === X",
    u8"X &&& X",
    u8"X ||| X",
    u8"V:",
    u8"V;",
    u8";V",
    u8":V",
    u8"func(I,)",
    u8"func(I, I,)",
    u8"func(I, I\n)",
    u8"I = X",
    u8"(I) = X",
    u8"(I + II) = X"
    u8"I + var = X",
    u8"(var) = X",
    u8"(var + I) = X",
    u8"func() = X"
));
