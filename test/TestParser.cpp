#include "TestParser.hpp"

// --- General section ---

std::string runParser(std::u8string& input) {
    std::vector<Token> token;
    std::ostringstream oss;
    std::ostringstream oss_dump;

    Lexer(input).tokenize(token, oss_dump);
    Parser parser(token, true, oss);
    auto block = parser.parse();

    if (block != nullptr) {
        block->printTree(oss, "", true);
    } else oss << "block is nullptr";

    return oss.str();
}

std::string runParserInvalid(std::u8string& input) {
    std::vector<Token> token;
    std::ostringstream oss;
    std::ostringstream oss_dump;

    Lexer(input).tokenize(token, oss_dump);
    Parser parser(token, true, oss);
    auto block = parser.parse();

    if (block != nullptr) {
        block->printTree(oss, "", true);
    } else oss << "block is nullptr";

    return parser.isValid() ? oss.str() : "INVALID";
}


// --- Expression section ---

INSTANTIATE_TEST_SUITE_P(TestParserExpressionValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"var = I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = (I)",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = (((I)))",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = func()",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"var = ((func()))",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"var = foo(I)",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── FuncCallAST(foo)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = foo(I, II)",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── FuncCallAST(foo)\n"
        "            ├── NumberAST(1)\n"
        "            └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"var = foo(I, II, III)",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── FuncCallAST(foo)\n"
        "            ├── NumberAST(1)\n"
        "            ├── NumberAST(2)\n"
        "            └── NumberAST(3)\n"
    ),
    std::make_pair(
        u8"var = foo((func() + I), X ⇔ X-III, (V))",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── FuncCallAST(foo)\n"
        "            ├── BinaryOperatorAST('+')\n"
        "            │   ├── FuncCallAST(func)\n"
        "            │   └── NumberAST(1)\n"
        "            ├── BinaryOperatorAST('⇔')\n"
        "            │   ├── NumberAST(10)\n"
        "            │   └── BinaryOperatorAST('-')\n"
        "            │       ├── NumberAST(10)\n"
        "            │       └── NumberAST(3)\n"
        "            └── NumberAST(5)\n"
    ),
    std::make_pair(
        u8"var = I + I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('+')\n"
        "            ├── NumberAST(1)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I - I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('-')\n"
        "            ├── NumberAST(1)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I × I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('×')\n"
        "            ├── NumberAST(1)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I ÷ I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('÷')\n"
        "            ├── NumberAST(1)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var = I + II × III",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('+')\n"
        "            ├── NumberAST(1)\n"
        "            └── BinaryOperatorAST('×')\n"
        "                ├── NumberAST(2)\n"
        "                └── NumberAST(3)\n"
    ),
    std::make_pair(
        u8"var = -var",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('-')\n"
        "            ├── NumberAST(0)\n"
        "            └── VariableReferenceAST(var)\n"
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
    u8"var = si I ⇔ I: ; nisi: ;",
    u8"var = si I ⇔ I: ; ni I ⇔ I: ;",
    u8"var = si I ⇔ I: ; ni I ⇔ I: si I ⇔ I: ;",
    u8"var = nisi - X",
    u8"var = X ⇔⇔ X",
    u8"var = X ∧∧ X",
    u8"var = X ∨∨ X",
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
    u8"var = I + (var = I)",
    u8"var = ++var",
    u8"var = var++",
    u8"var = --var",
    u8"var = var--"
));

// --- Declaration section ---

INSTANTIATE_TEST_SUITE_P(TestParserDeclarationValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"numerus var = I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableDeclarationAST(numerus var)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"numerus id = I + II × (III + IV)",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableDeclarationAST(numerus id)\n"
        "        └── BinaryOperatorAST('+')\n"
        "            ├── NumberAST(1)\n"
        "            └── BinaryOperatorAST('×')\n"
        "                ├── NumberAST(2)\n"
        "                └── BinaryOperatorAST('+')\n"
        "                    ├── NumberAST(3)\n"
        "                    └── NumberAST(4)\n"
    ),

    // Function Declaration
    std::make_pair(
        u8"nihil foo = λ(): ;",
        "└── BlockAST\n"
        "    └── FunctionAST\n"
        "        ├── FunctionPrototypeAST(nihil foo)\n"
        "        └── BlockAST\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(): \n ;",
        "└── BlockAST\n"
        "    └── FunctionAST\n"
        "        ├── FunctionPrototypeAST(nihil foo)\n"
        "        └── BlockAST\n"
    ),
    std::make_pair(
        u8"numerus foo = λ(): id = I + II ;",
        "└── BlockAST\n"
        "    └── FunctionAST\n"
        "        ├── FunctionPrototypeAST(numerus foo)\n"
        "        └── BlockAST\n"
        "            └── BinaryOperatorAST('=')\n"
        "                ├── VariableReferenceAST(id)\n"
        "                └── BinaryOperatorAST('+')\n"
        "                    ├── NumberAST(1)\n"
        "                    └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(numerus id): id = id + I ;",
        "└── BlockAST\n"
        "    └── FunctionAST\n"
        "        ├── FunctionPrototypeAST(nihil foo)\n"
        "        │   └── numerus id\n"
        "        └── BlockAST\n"
        "            └── BinaryOperatorAST('=')\n"
        "                ├── VariableReferenceAST(id)\n"
        "                └── BinaryOperatorAST('+')\n"
        "                    ├── VariableReferenceAST(id)\n"
        "                    └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(numerus id, numerus var): ;",
        "└── BlockAST\n"
        "    └── FunctionAST\n"
        "        ├── FunctionPrototypeAST(nihil foo)\n"
        "        │   ├── numerus id\n"
        "        │   └── numerus var\n"
        "        └── BlockAST\n"
    ),
    std::make_pair(
        u8"nihil foo = λ(numerus id, numerus var, numerus i): ;",
        "└── BlockAST\n"
        "    └── FunctionAST\n"
        "        ├── FunctionPrototypeAST(nihil foo)\n"
        "        │   ├── numerus id\n"
        "        │   ├── numerus var\n"
        "        │   └── numerus i\n"
        "        └── BlockAST\n"
    ),

    // Array declaration
    std::make_pair(
        u8"numerus[I] arr",
        "└── BlockAST\n"
        "    └── VariableDeclarationAST(numerus[1] arr)\n"
    ),
    std::make_pair(
        u8"numerus[I] arr = [I]",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableDeclarationAST(numerus[1] arr)\n"
        "        └── ArrayInitializationAST\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"numerus[II] a = [I, II]",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableDeclarationAST(numerus[2] a)\n"
        "        └── ArrayInitializationAST\n"
        "            ├── NumberAST(1)\n"
        "            └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"asertio[] a = [veri, falso]",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableDeclarationAST(asertio[2] a)\n"
        "        └── ArrayInitializationAST\n"
        "            ├── BoolAST(true)\n"
        "            └── BoolAST(false)\n"
    ),
    std::make_pair(
        u8"litera[II] a = ['a', 'b']",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableDeclarationAST(litera[2] a)\n"
        "        └── ArrayInitializationAST\n"
        "            ├── CharAST('a')\n"
        "            └── CharAST('b')\n"
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
    u8"nihil foo = λ(): \n nihil func = λ(): ; \n;",
    u8"nihil foo = λ(): nihil func = λ(): ; ;",

    // Array declaration
    u8"numerus[ foo = [I]",
    u8"numerus] foo = [I]",
    u8"numerus[] foo = []",
    u8"nihil[I] foo = [I]",
    u8"numerus[III] foo = [I, II, ]",
    u8"numerus[III] foo = []",
    u8"numerus[I] foo = [",
    u8"numerus[I] foo = ]",
    u8"numerus[I] foo = []",
    u8"numerus foo = [I, II, III]",
    u8"numerus[III] foo = [I, II, III",
    u8"numerus[III] foo = I, II, III",
    u8"numerus[III] foo = I",
    u8"litera[II] foo = ['a', ]",
    u8"litera[II] foo = 'a'",
    u8"litera[II] foo = (['a', 'b'])",
    u8"numerus[II] foo = ([I, II])",
    u8"numerus[II] foo = [I]",
    u8"numerus[II] foo = [I, II, III]",
    u8"numerus[-I] foo = [I]",
    u8"numerus[] foo = [I]",
    u8"numerus[-I] arr = [I]",
    u8"numerus[] arr = [I]",
    u8"numerus[O] arr = []",
    u8"numerus['a'] arr = []",
    u8"numerus[] arr",
    u8"numerus fake_arr = [I, II]"
));

// --- Assignment section ---

INSTANTIATE_TEST_SUITE_P(TestParserAssignmentValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"var = I",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"id = I + II × (III + IV)",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(id)\n"
        "        └── BinaryOperatorAST('+')\n"
        "            ├── NumberAST(1)\n"
        "            └── BinaryOperatorAST('×')\n"
        "                ├── NumberAST(2)\n"
        "                └── BinaryOperatorAST('+')\n"
        "                    ├── NumberAST(3)\n"
        "                    └── NumberAST(4)\n"
    ),
    std::make_pair(
        u8"id++",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(id)\n"
        "        └── BinaryOperatorAST('+')\n"
        "            ├── VariableReferenceAST(id)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"id--",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(id)\n"
        "        └── BinaryOperatorAST('-')\n"
        "            ├── VariableReferenceAST(id)\n"
        "            └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"var -= X",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('-')\n"
        "            ├── VariableReferenceAST(var)\n"
        "            └── NumberAST(10)\n"
    ),
    std::make_pair(
        u8"var ×= X",
        "└── BlockAST\n"
        "    └── BinaryOperatorAST('=')\n"
        "        ├── VariableReferenceAST(var)\n"
        "        └── BinaryOperatorAST('×')\n"
        "            ├── VariableReferenceAST(var)\n"
        "            └── NumberAST(10)\n"
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
    u8"var X",
    u8"var (X",
    u8"var = \n I",
    u8"var = I \n + I",
    u8"var = I + \n I",
    u8"var = numerus",
    u8"var = si I ⇔ I: ;",
    u8"var+++",
    u8"var---",
    u8"var××",
    u8"var++var = I",
    u8"var ⇔ var"
));

// --- Function call section ---

INSTANTIATE_TEST_SUITE_P(TestParserFunctionCallValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"func()",
        "└── BlockAST\n"
        "    └── FuncCallAST(func)\n"
    ),
    std::make_pair(
        u8"func(I)",
        "└── BlockAST\n"
        "    └── FuncCallAST(func)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"func((I))",
        "└── BlockAST\n"
        "    └── FuncCallAST(func)\n"
        "        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"foo(I, II)",
        "└── BlockAST\n"
        "    └── FuncCallAST(foo)\n"
        "        ├── NumberAST(1)\n"
        "        └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"foo(I + II, III)",
        "└── BlockAST\n"
        "    └── FuncCallAST(foo)\n"
        "        ├── BinaryOperatorAST('+')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        └── NumberAST(3)\n"
    ),
    std::make_pair(
        u8"func(foo())",
        "└── BlockAST\n"
        "    └── FuncCallAST(func)\n"
        "        └── FuncCallAST(foo)\n"
    ),
    std::make_pair(
        u8"func(foo(I))",
        "└── BlockAST\n"
        "    └── FuncCallAST(func)\n"
        "        └── FuncCallAST(foo)\n"
        "            └── NumberAST(1)\n"
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
    u8"func(, id)",
    u8"func(id, X, )",
    u8"func(id,",
    u8"func(id\n)",
    u8"func(id, X\n)",
    u8"func)id(",
    u8"func(si I ⇔ I: ;)",
    u8"func(I + I +)",
    u8"func(I = I)",
    u8"func(id = I)",
    u8"func id",
    u8"func id)",
    u8"func (id"
));

// --- Branching section ---

INSTANTIATE_TEST_SUITE_P(TestParserBranchingValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"si I ⇔ II: ;",
        "└── BlockAST\n"
        "    └── IfAST\n"
        "        ├── BinaryOperatorAST('⇔')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        ├── BlockAST\n"
        "        └── BlockAST\n"
    ),
    std::make_pair(
        u8"si I ⇔ II: var = I;",
        "└── BlockAST\n"
        "    └── IfAST\n"
        "        ├── BinaryOperatorAST('⇔')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        ├── BlockAST\n"
        "        │   └── BinaryOperatorAST('=')\n"
        "        │       ├── VariableReferenceAST(var)\n"
        "        │       └── NumberAST(1)\n"
        "        └── BlockAST\n"
    ),
    std::make_pair(
        u8"si I ⇔ II: var = I; ni: var = II;",
        "└── BlockAST\n"
        "    └── IfAST\n"
        "        ├── BinaryOperatorAST('⇔')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        ├── BlockAST\n"
        "        │   └── BinaryOperatorAST('=')\n"
        "        │       ├── VariableReferenceAST(var)\n"
        "        │       └── NumberAST(1)\n"
        "        └── BlockAST\n"
        "            └── BinaryOperatorAST('=')\n"
        "                ├── VariableReferenceAST(var)\n"
        "                └── NumberAST(2)\n"
    ),
    std::make_pair(
        u8"si I ⇔ II: var = I; nisi II ⇔ III: var = II; ni: var = III;",
        "└── BlockAST\n"
        "    └── IfAST\n"
        "        ├── BinaryOperatorAST('⇔')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        ├── BlockAST\n"
        "        │   └── BinaryOperatorAST('=')\n"
        "        │       ├── VariableReferenceAST(var)\n"
        "        │       └── NumberAST(1)\n"
        "        └── BlockAST\n"
        "            └── IfAST\n"
        "                ├── BinaryOperatorAST('⇔')\n"
        "                │   ├── NumberAST(2)\n"
        "                │   └── NumberAST(3)\n"
        "                ├── BlockAST\n"
        "                │   └── BinaryOperatorAST('=')\n"
        "                │       ├── VariableReferenceAST(var)\n"
        "                │       └── NumberAST(2)\n"
        "                └── BlockAST\n"
        "                    └── BinaryOperatorAST('=')\n"
        "                        ├── VariableReferenceAST(var)\n"
        "                        └── NumberAST(3)\n"
    ),
    std::make_pair(
        u8"si I ⇔ II: ; nisi II ⇔ III: ; nisi III ⇔ IV: ; nisi IV ⇔ V: ; ni: ;",
        "└── BlockAST\n"
        "    └── IfAST\n"
        "        ├── BinaryOperatorAST('⇔')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        ├── BlockAST\n"
        "        └── BlockAST\n"
        "            └── IfAST\n"
        "                ├── BinaryOperatorAST('⇔')\n"
        "                │   ├── NumberAST(2)\n"
        "                │   └── NumberAST(3)\n"
        "                ├── BlockAST\n"
        "                └── BlockAST\n"
        "                    └── IfAST\n"
        "                        ├── BinaryOperatorAST('⇔')\n"
        "                        │   ├── NumberAST(3)\n"
        "                        │   └── NumberAST(4)\n"
        "                        ├── BlockAST\n"
        "                        └── BlockAST\n"
        "                            └── IfAST\n"
        "                                ├── BinaryOperatorAST('⇔')\n"
        "                                │   ├── NumberAST(4)\n"
        "                                │   └── NumberAST(5)\n"
        "                                ├── BlockAST\n"
        "                                └── BlockAST\n"
    ),
    std::make_pair(
        u8"si I ⇔ II: ; nisi II ⇔ III: ; nisi III ⇔ IV: ; nisi IV ⇔ V: ;",
        "└── BlockAST\n"
        "    └── IfAST\n"
        "        ├── BinaryOperatorAST('⇔')\n"
        "        │   ├── NumberAST(1)\n"
        "        │   └── NumberAST(2)\n"
        "        ├── BlockAST\n"
        "        └── BlockAST\n"
        "            └── IfAST\n"
        "                ├── BinaryOperatorAST('⇔')\n"
        "                │   ├── NumberAST(2)\n"
        "                │   └── NumberAST(3)\n"
        "                ├── BlockAST\n"
        "                └── BlockAST\n"
        "                    └── IfAST\n"
        "                        ├── BinaryOperatorAST('⇔')\n"
        "                        │   ├── NumberAST(3)\n"
        "                        │   └── NumberAST(4)\n"
        "                        ├── BlockAST\n"
        "                        └── BlockAST\n"
        "                            └── IfAST\n"
        "                                ├── BinaryOperatorAST('⇔')\n"
        "                                │   ├── NumberAST(4)\n"
        "                                │   └── NumberAST(5)\n"
        "                                ├── BlockAST\n"
        "                                └── BlockAST\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserBranchingInvalid, TestParserInvalid, ::testing::Values(
    u8"si",
    u8"si I ⇔ I",
    u8"si I ⇔ I: ",
    u8"si: ;",
    u8"ni: ;",
    u8"nisi I ⇔ I: ;",
    u8"nisi: ;",
    u8"si I ⇔ I: ; nisi: ;",
    u8"si I ⇔ I: ; nisi I ⇔ I",
    u8"si I ⇔ I: ; nisi I ⇔ I:",
    u8"si I ⇔ I: ; nisi I ⇔ I;",
    u8"si I ⇔ I: ; nisi I ⇔ I: ni: ;",
    u8"si I ⇔ I: ; nisi I ⇔ I: ; ni:",
    u8"si I ⇔ I: ; nisi I ⇔ I: ; ni;",
    u8"si I ⇔ I: ; ni I ⇔ I: ;",
    u8"si I ⇔ I: ; nisi I ⇔ I: nisi I ⇔ I: ;",
    u8"si finio: ;",
    u8"si si I ⇔ I: ;",
    u8"si numerus var = I: ;",
    u8"si var = I: ;",
    u8"si (var = I): ;"
));

// --- Looping section ---

INSTANTIATE_TEST_SUITE_P(TestParserLoopingValid, TestParserValid, ::testing::Values(
    std::make_pair(
        u8"∑(): ;",
        "└── BlockAST\n"
        "    └── LoopAST\n"
        "        └── BlockAST\n"
    ),
    std::make_pair(
        u8"∑(var > X): ;",
        "└── BlockAST\n"
        "    └── LoopAST\n"
        "        └── BlockAST\n"
        "            └── IfAST\n"
        "                ├── BinaryOperatorAST('>')\n"
        "                │   ├── VariableReferenceAST(var)\n"
        "                │   └── NumberAST(10)\n"
        "                ├── BlockAST\n"
        "                └── BlockAST\n"
        "                    └── BreakAST\n"
    ),
    std::make_pair(
        u8"∑(i > V, i = i + I): ;",
        "└── BlockAST\n"
        "    └── LoopAST\n"
        "        └── BlockAST\n"
        "            ├── IfAST\n"
        "            │   ├── BinaryOperatorAST('>')\n"
        "            │   │   ├── VariableReferenceAST(i)\n"
        "            │   │   └── NumberAST(5)\n"
        "            │   ├── BlockAST\n"
        "            │   └── BlockAST\n"
        "            │       └── BreakAST\n"
        "            └── BinaryOperatorAST('=')\n"
        "                ├── VariableReferenceAST(i)\n"
        "                └── BinaryOperatorAST('+')\n"
        "                    ├── VariableReferenceAST(i)\n"
        "                    └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"∑(i > V, i++): ;",
        "└── BlockAST\n"
        "    └── LoopAST\n"
        "        └── BlockAST\n"
        "            ├── IfAST\n"
        "            │   ├── BinaryOperatorAST('>')\n"
        "            │   │   ├── VariableReferenceAST(i)\n"
        "            │   │   └── NumberAST(5)\n"
        "            │   ├── BlockAST\n"
        "            │   └── BlockAST\n"
        "            │       └── BreakAST\n"
        "            └── BinaryOperatorAST('=')\n"
        "                ├── VariableReferenceAST(i)\n"
        "                └── BinaryOperatorAST('+')\n"
        "                    ├── VariableReferenceAST(i)\n"
        "                    └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"∑(numerus i = I): ;",
        "└── BlockAST\n"
        "    └── BlockAST\n"
        "        ├── BinaryOperatorAST('=')\n"
        "        │   ├── VariableDeclarationAST(numerus i)\n"
        "        │   └── NumberAST(1)\n"
        "        └── LoopAST\n"
        "            └── BlockAST\n"
    ),
    std::make_pair(
        u8"∑(numerus j = V, j > X): ;",
        "└── BlockAST\n"
        "    └── BlockAST\n"
        "        ├── BinaryOperatorAST('=')\n"
        "        │   ├── VariableDeclarationAST(numerus j)\n"
        "        │   └── NumberAST(5)\n"
        "        └── LoopAST\n"
        "            └── BlockAST\n"
        "                └── IfAST\n"
        "                    ├── BinaryOperatorAST('>')\n"
        "                    │   ├── VariableReferenceAST(j)\n"
        "                    │   └── NumberAST(10)\n"
        "                    ├── BlockAST\n"
        "                    └── BlockAST\n"
        "                        └── BreakAST\n"
    ),
    std::make_pair(
        u8"∑(numerus i = I, i > X, i = i + I): ;",
        "└── BlockAST\n"
        "    └── BlockAST\n"
        "        ├── BinaryOperatorAST('=')\n"
        "        │   ├── VariableDeclarationAST(numerus i)\n"
        "        │   └── NumberAST(1)\n"
        "        └── LoopAST\n"
        "            └── BlockAST\n"
        "                ├── IfAST\n"
        "                │   ├── BinaryOperatorAST('>')\n"
        "                │   │   ├── VariableReferenceAST(i)\n"
        "                │   │   └── NumberAST(10)\n"
        "                │   ├── BlockAST\n"
        "                │   └── BlockAST\n"
        "                │       └── BreakAST\n"
        "                └── BinaryOperatorAST('=')\n"
        "                    ├── VariableReferenceAST(i)\n"
        "                    └── BinaryOperatorAST('+')\n"
        "                        ├── VariableReferenceAST(i)\n"
        "                        └── NumberAST(1)\n"
    ),
    std::make_pair(
        u8"∑(): finio ;",
        "└── BlockAST\n"
        "    └── LoopAST\n"
        "        └── BlockAST\n"
        "            └── BreakAST\n"
    ),
    std::make_pair(
        u8"∑(): si I ⇔ II: finio ; ;",
        "└── BlockAST\n"
        "    └── LoopAST\n"
        "        └── BlockAST\n"
        "            └── IfAST\n"
        "                ├── BinaryOperatorAST('⇔')\n"
        "                │   ├── NumberAST(1)\n"
        "                │   └── NumberAST(2)\n"
        "                ├── BlockAST\n"
        "                │   └── BreakAST\n"
        "                └── BlockAST\n"
    )
));


INSTANTIATE_TEST_SUITE_P(TestParserLoopingInvalid, TestParserInvalid, ::testing::Values(
    u8"∑",
    u8"∑(",
    u8"∑))",
    u8"∑()",
    u8"∑():",
    u8"∑();",
    u8"finio",
    u8"∑(numerus);",
    u8"∑(V ⇔ V, numerus var);",
    u8"∑(V ⇔ V, numerus var = I);",
    u8"∑(finio);",
    u8"∑(func(): ;",
    u8"∑(X ⇔ X, ): ;",
    u8"∑(X ⇔ X, var = V, ): ;",
    u8"∑(X ⇔ X, var = V: ;",
    u8"∑(X ⇔ X: ;",
    u8"∑(,X ⇔ X): ;",
    u8"∑(X ⇔ X, var = I,): ;",
    u8"∑(var = I): ;",
    u8"∑(numerus var = I, var = I): ;",
    u8"∑(var = I, numerus i = I): ;",
    u8"∑(var ⇔ I, numerus i = I): ;"
));
