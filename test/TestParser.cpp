#include "TestParser.hpp"

// --- General section ---

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


// --- Expression section ---

INSTANTIATE_TEST_SUITE_P(TestParserExpressionValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"var = I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = (I)",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = (((I)))",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = func()",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"var = ((func()))",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"var = foo(I)",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── FuncCallAST(foo)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = foo(I, I)",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── FuncCallAST(foo)\n"
        "│           ├── NumberAST(1)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = foo(I, I, I)",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── FuncCallAST(foo)\n"
        "│           ├── NumberAST(1)\n"
        "│           ├── NumberAST(1)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = foo((func() + I), X == X-III, (V))",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── FuncCallAST(foo)\n"
        "│           ├── BinaryOperatorAST('+')\n"
        "│           │   ├── FuncCallAST(func)\n"
        "│           │   └── NumberAST(1)\n"
        "│           ├── BinaryOperatorAST('==')\n"
        "│           │   ├── NumberAST(10)\n"
        "│           │   └── BinaryOperatorAST('-')\n"
        "│           │       ├── NumberAST(10)\n"
        "│           │       └── NumberAST(3)\n"
        "│           └── NumberAST(5)\n"
    ),
    std::make_pair(
        u8"var = I + I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── BinaryOperatorAST('+')\n"
        "│           ├── NumberAST(1)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I - I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── BinaryOperatorAST('-')\n"
        "│           ├── NumberAST(1)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I * I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── BinaryOperatorAST('*')\n"
        "│           ├── NumberAST(1)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I / I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── BinaryOperatorAST('/')\n"
        "│           ├── NumberAST(1)\n"
        "│           └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I + II * III",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── BinaryOperatorAST('*')\n"
        "│           ├── BinaryOperatorAST('+')\n"
        "│           │   ├── NumberAST(1)\n"
        "│           │   └── NumberAST(2)\n"
        "│           └── NumberAST(3)\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserExpressionInvalid, TestParserInvalid, ::testing::Values(
    u8"var = func(I,)",
    u8"var = ()",
    u8"var = I +",
    u8"var = I + (I",
    u8"var = I + I + ",
    u8"var = I + (I + I",
    u8"var = I + (I + I) +",
    u8"var = I + (I +",
    u8"var = print(",
    u8"var = finio + III",
    u8"var = si - func()",
    u8"var = si: ;",
    u8"var = si I == I: ; nisi: ;",
    u8"var = si I == I: ; ni I == I: ;",
    u8"var = si I == I: ; ni I == I: si I == I: ;",
    u8"var = nisi - X",
    u8"var = X === X",
    u8"var = X &&& X",
    u8"var = X ||| X",
    u8"var = V:",
    u8"var = V;",
    u8"var = ;V",
    u8"var = :V",
    u8"var = func(I,)",
    u8"var = func(I, I,)",
    u8"var = func(I, I\n)",
    u8"var = I = X",
    u8"var = (I) = X",
    u8"var = (I + II) = X"
    u8"var = I + var = X",
    u8"var = (var) = X",
    u8"var = (var + I) = X",
    u8"var = func() = X",
    u8"var = (var = I)",
    u8"var = (var = I) + I",
    u8"var = I + (var = I)"
));

// --- Declaration section ---

INSTANTIATE_TEST_SUITE_P(TestParserDeclarationValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"numerus var = I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableDeclarationAST(numerus var)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"numerus id = I + II * (III + IV)",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableDeclarationAST(numerus id)\n"
        "│       └── BinaryOperatorAST('*')\n"
        "│           ├── BinaryOperatorAST('+')\n"
        "│           │   ├── NumberAST(1)\n"
        "│           │   └── NumberAST(2)\n"
        "│           └── BinaryOperatorAST('+')\n"
        "│               ├── NumberAST(3)\n"
        "│               └── NumberAST(4)\n"
    ),
    // Function Declaration
    std::make_pair(
        u8"nihil foo = λ(): ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       └── BlockAST\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(): \n ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       └── BlockAST\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(): id = I + II ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       └── BlockAST\n"
        "│           └── BinaryOperatorAST('=')\n"
        "│               ├── VariableReferenceAST(id)\n"
        "│               └── BinaryOperatorAST('+')\n"
        "│                   ├── NumberAST(1)\n"
        "│                   └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(numerus id): id = id + I ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       └── VariableDeclarationAST(numerus id)\n"
        "│       └── BlockAST\n"
        "│           └── BinaryOperatorAST('=')\n"
        "│               ├── VariableReferenceAST(id)\n"
        "│               └── BinaryOperatorAST('+')\n"
        "│                   ├── VariableReferenceAST(id)\n"
        "│                   └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(numerus id, numerus var): ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       ├── VariableDeclarationAST(numerus id)\n"
        "│       └── VariableDeclarationAST(numerus var)\n"
        "│       └── BlockAST\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(numerus id, numerus var, numerus i): ;",
        "├── BlockAST\n"
        "│   └── FunctionAST(nihil foo)\n"
        "│       ├── VariableDeclarationAST(numerus id)\n"
        "│       ├── VariableDeclarationAST(numerus var)\n"
        "│       └── VariableDeclarationAST(numerus i)\n"
        "│       └── BlockAST\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserDeclarationInvalid, TestParserInvalid, ::testing::Values(
    u8"numerus X = I",
    u8"numerus numerus = I",
    u8"numerus si = I",
    u8"numerus nisi = I",
    u8"numerus finio = I",
    u8"numerus = I",
    u8"numerus",
    u8"numerus =",
    u8"func() = I",
    u8"= I",
    u8"=",
    u8"(numerus = I)",
    u8"I + (numerus = I)",
    u8"(numerus = I) + I",
    u8"si numerus = I: ;",
    u8"si numerus var = I: ;",

    // Function Declaration
    u8"λ",
    u8"λ(): ;",
    u8"nihil foo = (numerus j)",
    u8"nihil foo = (numerus j): ;",
    u8"nihil foo = λ",
    u8"nihil foo = λ(",
    u8"nihil foo = λ()",
    u8"nihil foo = λ():",
    u8"nihil foo = λ();",
    u8"nihil foo = λ:",
    u8"nihil foo = λ;",
    u8"nihil foo = λ(I): ;",
    u8"nihil foo = λ(numerus): ;",
    u8"nihil foo = λ(λ): ;",
    u8"nihil foo = λ(id, ): ;",
    u8"nihil foo = λ id: ;",
    u8"nihil foo = λ(id: ;",
    u8"nihil foo = λ id): ;",
    u8"nihil foo = λ(id): ;",
    u8"nihil foo = λ(numerus id,: ;",
    u8"nihil foo = λ(numerus id,): ;",
    u8"nihil foo = λ(numerus id, : ;",
    u8"nihil foo = λ(numerus i, numerus j,): ;",
    u8"nihil foo = λ(numerus i, numerus j,): ;",
    u8"nihil foo = λ(numerus i, numerus j): i = i+ ;",
    u8"nihil foo = (λ(): ;)",
    u8""
));

// --- Assignment section ---

INSTANTIATE_TEST_SUITE_P(TestParserAssignmentValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"var = I",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(var)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"id = I + II * (III + IV)",
        "├── BlockAST\n"
        "│   └── BinaryOperatorAST('=')\n"
        "│       ├── VariableReferenceAST(id)\n"
        "│       └── BinaryOperatorAST('*')\n"
        "│           ├── BinaryOperatorAST('+')\n"
        "│           │   ├── NumberAST(1)\n"
        "│           │   └── NumberAST(2)\n"
        "│           └── BinaryOperatorAST('+')\n"
        "│               ├── NumberAST(3)\n"
        "│               └── NumberAST(4)\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserAssignmentInvalid, TestParserInvalid, ::testing::Values(
    u8"X = I",
    u8"func() = I",
    u8"I = func()",
    u8"var =",
    u8"var = func(",
    u8"var = func(I,",
    u8"var = func(I, II",
    u8"var = I +",
    u8"var (= I)",
    u8"var := I",
    u8"var := I;",
    u8"var -= X",
    u8"var X",
    u8"var (X",
    u8"var = \n I",
    u8"var = I \n + I",
    u8"var = I + \n I",
    u8"var = numerus",
    u8"var = si I == I: ;",
    u8"var = finio"
));

// --- Function call section ---

INSTANTIATE_TEST_SUITE_P(TestParserFunctionCallValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"func()",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"func(I)",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"func((I))",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
        "│       └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"foo(I, II)",
        "├── BlockAST\n"
        "│   └── FuncCallAST(foo)\n"
        "│       ├── NumberAST(1)\n"
        "│       └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"foo(I + II, III)",
        "├── BlockAST\n"
        "│   └── FuncCallAST(foo)\n"
        "│       ├── BinaryOperatorAST('+')\n"
        "│       │   ├── NumberAST(1)\n"
        "│       │   └── NumberAST(2)\n"
        "│       └── NumberAST(3)\n"
    ),
    std::make_pair(
        u8"func(foo())",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
        "│       └── FuncCallAST(foo)\n"
    ),
    std::make_pair(
        u8"func(foo(I))",
        "├── BlockAST\n"
        "│   └── FuncCallAST(func)\n"
        "│       └── FuncCallAST(foo)\n"
        "│           └── NumberAST(1)\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserFunctionCallInvalid, TestParserInvalid, ::testing::Values(
    u8"func(",
    u8"func)",
    u8"func(,)",
    u8"func(numerus)",
    u8"func(numerus id = X)",
    u8"func(X, )",
    u8"func(id, )",
    u8"func(id, X, )",
    u8"func(id,",
    u8"func(id\n)",
    u8"func(id, X\n)",
    u8"func)id(",
    u8"func(si I == I: ;)",
    u8"func(I + I +)",
    u8"func(I = I)",
    u8"func(id = I)",
    u8"func id",
    u8"func id)",
    u8"func (id"
));
