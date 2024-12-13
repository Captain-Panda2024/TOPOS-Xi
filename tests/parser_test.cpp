#include <gtest/gtest.h>
#include "compiler/parser/parser.hpp"
#include "compiler/lexer/lexer.hpp"

using namespace topos::compiler;
using namespace topos::ast;

TEST(ParserTest, BasicParsing) {
    std::string input = "space MySpace { property x: Topology<int> = y; }";
    Lexer lexer{llvm::StringRef(input)};
    
    // Collect all tokens from the lexer
    std::vector<Token> tokens;
    while (true) {
        Token token = lexer.nextToken();
        tokens.push_back(token);
        if (token.getType() == Token::Type::EndOfFile) break;
    }
    
    Parser parser{tokens};

    auto ast = parser.parseProgram();
    ASSERT_NE(ast, nullptr);

    // プログラムノードの検証
    auto* program = dynamic_cast<ProgramNode*>(ast.get());
    ASSERT_NE(program, nullptr);

    // スペース宣言のチェック
    ASSERT_EQ(program->getSpaces().size(), 1);
    auto* space = program->getSpaces()[0].get();
    ASSERT_NE(space, nullptr);

    // スペース名の検証
    auto* name = space->getName();
    ASSERT_NE(name, nullptr);
    EXPECT_EQ(name->getName(), "MySpace");

    // プロパティの検証
    ASSERT_EQ(space->getProperties().size(), 1);
    auto* property = space->getProperties()[0].get();
    ASSERT_NE(property, nullptr);

    // プロパティ名の検証
    auto* prop_name = property->getName();
    ASSERT_NE(prop_name, nullptr);
    EXPECT_EQ(prop_name->getName(), "x");

    // プロパティの型の検証
    auto* type = property->getType();
    ASSERT_NE(type, nullptr);
    EXPECT_EQ(type->getKind(), TypeNode::TypeKind::Topology);
}
