#include <gtest/gtest.h>
#include "compiler/lexer/lexer.hpp"

using namespace topos::compiler;

TEST(LexerTest, BasicTokenization) {
    std::string input = "space MySpace { }";
    Lexer lexer{llvm::StringRef(input)};

    auto token1 = lexer.nextToken();
    EXPECT_EQ(token1.getType(), Token::Type::Space);
    EXPECT_EQ(token1.getText(), "space");

    auto token2 = lexer.nextToken();
    EXPECT_EQ(token2.getType(), Token::Type::Identifier);
    EXPECT_EQ(token2.getText(), "MySpace");

    auto token3 = lexer.nextToken();
    EXPECT_EQ(token3.getType(), Token::Type::LeftBrace);

    auto token4 = lexer.nextToken();
    EXPECT_EQ(token4.getType(), Token::Type::RightBrace);

    auto token5 = lexer.nextToken();
    EXPECT_EQ(token5.getType(), Token::Type::EndOfFile);
}
