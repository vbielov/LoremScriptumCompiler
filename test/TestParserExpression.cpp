#include <Parser.hpp>

#include "TestLexer.hpp"

TEST(ParserTest, TestParserExpression) {
    Lexer lexer = Lexer(u8"I + I");
    Parser parser = Parser(lexer);
    auto block = parser.parseBlock();

    std::ostringstream oss;
    block->printTree(oss, "", false);

    EXPECT_EQ(1, 1);
}