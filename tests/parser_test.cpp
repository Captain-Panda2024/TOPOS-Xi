#include <gtest/gtest.h>
#include "compiler/parser/parser.hpp"

using namespace topos::compiler;

TEST(ParserTest, BasicParsing) {
    std::string input = R"(
        space MySpace {
            shape Point {}
            mapping f: Point -> Point
        }
    )";

    Parser parser(input);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    // プログラムノードのチェック
    auto* program = dynamic_cast<ProgramNode*>(ast.get());
    ASSERT_NE(program, nullptr);

    // スペース宣言のチェック
    ASSERT_EQ(program->getSpaces().size(), 1);
    auto* space = program->getSpaces()[0];
    EXPECT_EQ(space->getName(), "MySpace");

    // シェイプ宣言のチェック
    ASSERT_EQ(space->getShapes().size(), 1);
    auto* shape = space->getShapes()[0];
    EXPECT_EQ(shape->getName(), "Point");

    // マッピング宣言のチェック
    ASSERT_EQ(space->getMappings().size(), 1);
    auto* mapping = space->getMappings()[0];
    EXPECT_EQ(mapping->getName(), "f");
}
