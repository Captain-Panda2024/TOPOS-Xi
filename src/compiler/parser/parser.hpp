#ifndef TOPOS_COMPILER_PARSER_PARSER_HPP
#define TOPOS_COMPILER_PARSER_PARSER_HPP

#include "topos/compiler/lexer.hpp"
#include "topos/ast/ast.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

namespace topos {
namespace compiler {

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
    explicit Parser(Lexer& lexer)
        : lexer_(lexer), current_(lexer.nextToken()) {}

    std::unique_ptr<ast::ProgramNode> parseProgram() {
        std::vector<std::unique_ptr<ast::SpaceNode>> spaces;
        
        while (current_.getType() != Token::Type::EndOfFile) {
            if (current_.getType() == Token::Type::Space) {
                spaces.push_back(parseSpace());
            } else {
                throw ParseError("Expected 'space' declaration",
                               Location(current_.getLine(), current_.getColumn()));
            }
        }

        return ast::ASTBuilder().buildProgram(std::move(spaces));
    }

private:
    Token consume(Token::Type type) {
        if (current_.getType() != type) {
            throw ParseError(
                "Unexpected token: expected " + std::string(tokenTypeToString(type)) +
                ", got " + std::string(tokenTypeToString(current_.getType())),
                Location(current_.getLine(), current_.getColumn()));
        }

        Token token = current_;
        current_ = lexer_.nextToken();
        return token;
    }

    bool match(Token::Type type) {
        if (current_.getType() != type) return false;
        current_ = lexer_.nextToken();
        return true;
    }

    std::unique_ptr<ast::SpaceNode> parseSpace() {
        consume(Token::Type::Space);
        auto name = parseIdentifier();
        consume(Token::Type::LeftBrace);

        std::vector<std::unique_ptr<ast::PropertyNode>> properties;
        std::vector<std::unique_ptr<ast::ShapeNode>> shapes;

        while (!match(Token::Type::RightBrace)) {
            if (match(Token::Type::Property)) {
                properties.push_back(parseProperty());
            } else if (match(Token::Type::Shape)) {
                shapes.push_back(parseShape());
            } else if (match(Token::Type::Invariant)) {
                properties.push_back(parseInvariant());
            } else {
                throw ParseError("Expected property, shape, or invariant declaration",
                               Location(current_.getLine(), current_.getColumn()));
            }
        }

        return ast::ASTBuilder().buildSpace(
            std::move(name),
            std::move(properties),
            std::move(shapes));
    }

    std::unique_ptr<ast::ShapeNode> parseShape() {
        auto name = parseIdentifier();
        consume(Token::Type::LeftBrace);

        std::vector<std::unique_ptr<ast::PropertyNode>> properties;
        std::vector<std::unique_ptr<ast::MappingNode>> mappings;

        while (!match(Token::Type::RightBrace)) {
            if (match(Token::Type::Property)) {
                properties.push_back(parseProperty());
            } else if (match(Token::Type::Mapping)) {
                mappings.push_back(parseMapping());
            } else if (match(Token::Type::Invariant)) {
                properties.push_back(parseInvariant());
            } else {
                throw ParseError("Expected property, mapping, or invariant declaration",
                               Location(current_.getLine(), current_.getColumn()));
            }
        }

        return ast::ASTBuilder().buildShape(
            std::move(name),
            std::move(properties),
            std::move(mappings));
    }

    std::unique_ptr<ast::MappingNode> parseMapping() {
        auto name = parseIdentifier();
        consume(Token::Type::LeftBrace);

        std::vector<std::unique_ptr<ast::PropertyNode>> properties;
        std::vector<std::unique_ptr<ast::ASTNode>> body;

        while (!match(Token::Type::RightBrace)) {
            if (match(Token::Type::Property)) {
                properties.push_back(parseProperty());
            } else if (match(Token::Type::Path)) {
                body.push_back(parsePath());
            } else if (match(Token::Type::Invariant)) {
                properties.push_back(parseInvariant());
            } else {
                throw ParseError("Expected property, path, or invariant declaration",
                               Location(current_.getLine(), current_.getColumn()));
            }
        }

        return ast::ASTBuilder().buildMapping(
            std::move(name),
            std::move(properties),
            std::move(body));
    }

    std::unique_ptr<ast::PropertyNode> parseProperty() {
        auto name = parseIdentifier();
        consume(Token::Type::Colon);
        auto type = parseType();
        
        std::unique_ptr<ast::ASTNode> value;
        if (match(Token::Type::Equal)) {
            value = parseExpression();
        }

        consume(Token::Type::Semicolon);
        return ast::ASTBuilder().buildProperty(
            std::move(name),
            std::move(type),
            std::move(value));
    }

    std::unique_ptr<ast::InvariantNode> parseInvariant() {
        auto name = parseIdentifier();
        consume(Token::Type::Colon);
        auto type = parseType();
        consume(Token::Type::Equal);
        auto condition = parseExpression();
        consume(Token::Type::Semicolon);

        return ast::ASTBuilder().buildInvariant(
            std::move(name),
            std::move(type),
            std::move(condition));
    }

    std::unique_ptr<ast::PathNode> parsePath() {
        std::vector<std::unique_ptr<ast::PathElementNode>> elements;

        while (true) {
            elements.push_back(parsePathElement());
            if (!match(Token::Type::Arrow)) break;
        }

        return ast::ASTBuilder().buildPath(std::move(elements));
    }

    std::unique_ptr<ast::PathElementNode> parsePathElement() {
        return ast::ASTBuilder().buildPathElement(parseIdentifier());
    }

    std::unique_ptr<ast::IdentifierNode> parseIdentifier() {
        Token token = consume(Token::Type::Identifier);
        return ast::ASTBuilder().buildIdentifier(
            token.getText().str(),
            Location(token.getLine(), token.getColumn()));
    }

    std::unique_ptr<ast::TypeNode> parseType() {
        Token token = current_;
        consume(Token::Type::Identifier);

        ast::TypeNode::TypeKind kind;
        if (token.getText() == "Topology") {
            kind = ast::TypeNode::TypeKind::Topology;
            consume(Token::Type::LessThan);
            auto baseType = parseType();
            consume(Token::Type::GreaterThan);
            return ast::ASTBuilder().buildType(kind, std::move(baseType));
        } else if (token.getText() == "Quantum") {
            kind = ast::TypeNode::TypeKind::Quantum;
            consume(Token::Type::LessThan);
            auto baseType = parseType();
            consume(Token::Type::GreaterThan);
            return ast::ASTBuilder().buildType(kind, std::move(baseType));
        } else {
            kind = ast::TypeNode::TypeKind::Basic;
            return ast::ASTBuilder().buildType(kind);
        }
    }

    std::unique_ptr<ast::ASTNode> parseExpression() {
        // 式の解析は現時点では単純な識別子のみをサポート
        return parseIdentifier();
    }

    static const char* tokenTypeToString(Token::Type type) {
        switch (type) {
            case Token::Type::Space: return "space";
            case Token::Type::Shape: return "shape";
            case Token::Type::Mapping: return "mapping";
            case Token::Type::Property: return "property";
            case Token::Type::Path: return "path";
            case Token::Type::Invariant: return "invariant";
            case Token::Type::Identifier: return "identifier";
            case Token::Type::LeftBrace: return "{";
            case Token::Type::RightBrace: return "}";
            case Token::Type::Arrow: return "->";
            case Token::Type::Colon: return ":";
            case Token::Type::Semicolon: return ";";
            case Token::Type::Equal: return "=";
            case Token::Type::LessThan: return "<";
            case Token::Type::GreaterThan: return ">";
            case Token::Type::EndOfFile: return "EOF";
            default: return "unknown";
        }
    }

    Lexer& lexer_;
    Token current_;
};

} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_PARSER_PARSER_HPP
