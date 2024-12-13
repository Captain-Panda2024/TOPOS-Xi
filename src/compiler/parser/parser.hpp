#ifndef TOPOS_COMPILER_PARSER_PARSER_HPP
#define TOPOS_COMPILER_PARSER_PARSER_HPP

#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

namespace topos::compiler {

using namespace topos::ast;

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, Location location)
        : std::runtime_error(message), location_(location) {}

    Location getLocation() const { return location_; }

private:
    Location location_;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens)
        : tokens_(tokens), current_(0) {}

    std::unique_ptr<ProgramNode> parseProgram();

private:
    bool match(Token::Type type);
    Token consume(Token::Type type, const std::string& message);
    bool check(Token::Type type);
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();

    std::unique_ptr<SpaceNode> parseSpace();
    std::unique_ptr<PropertyNode> parseProperty();
    std::unique_ptr<TypeNode> parseType();
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<IdentifierNode> parseIdentifier();
    std::unique_ptr<ExpressionNode> parseNumber();
    std::unique_ptr<ExpressionNode> parseString();
    std::unique_ptr<ExpressionNode> parseBinaryExpr();
    std::unique_ptr<ExpressionNode> parseUnaryExpr();
    std::unique_ptr<ExpressionNode> parseCallExpr();
    std::unique_ptr<ShapeNode> parseShape();
    std::unique_ptr<MappingNode> parseMapping();

    std::vector<Token> tokens_;
    size_t current_;
};

} // namespace topos::compiler

#endif // TOPOS_COMPILER_PARSER_PARSER_HPP
