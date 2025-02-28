#include "TestParser.hpp"

INSTANTIATE_TEST_SUITE_P(TestParserExpressionValid, ParserTestValid, ::testing::Values(
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
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserExpressionInvalid, ParserTestInvalid, ::testing::Values(
    u8"I + (I",
    u8"I + I + "
));
