#include "parser.hpp"

namespace topos::compiler {

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    std::vector<std::unique_ptr<SpaceNode>> spaces;
    while (current_ < tokens_.size() && !isAtEnd()) {
        if (match(Token::Type::Space)) {
            spaces.push_back(parseSpace());
        } else {
            throw ParseError("Expected 'space' declaration", Location(tokens_[current_].getLine(), tokens_[current_].getColumn()));
        }
    }
    return std::make_unique<ProgramNode>(std::move(spaces));
}

bool Parser::match(Token::Type type) {
    if (isAtEnd()) return false;
    if (tokens_[current_].getType() != type) return false;
    current_++;
    return true;
}

Token Parser::consume(Token::Type type, const std::string& message) {
    if (check(type)) {
        current_++;
        return tokens_[current_ - 1];
    }
    throw ParseError(message, Location(tokens_[current_].getLine(), tokens_[current_].getColumn()));
}

bool Parser::check(Token::Type type) {
    if (isAtEnd()) return false;
    return tokens_[current_].getType() == type;
}

bool Parser::isAtEnd() {
    return current_ >= tokens_.size();
}

Token Parser::peek() {
    if (isAtEnd()) {
        return Token(Token::Type::EndOfFile, llvm::StringRef(""), 0, 0);
    }
    return tokens_[current_];
}

Token Parser::previous() {
    return tokens_[current_ - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

std::unique_ptr<SpaceNode> Parser::parseSpace() {
    // Space キーワードは既に消費済み
    auto name = parseIdentifier();
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<ShapeNode>> shapes;

    if (match(Token::Type::LeftBrace)) {
        while (!isAtEnd() && !match(Token::Type::RightBrace)) {
            if (match(Token::Type::Property)) {
                properties.push_back(parseProperty());
            } else if (match(Token::Type::Shape)) {
                shapes.push_back(parseShape());
            } else {
                throw ParseError("Expected property or shape declaration",
                               Location(tokens_[current_].getLine(), tokens_[current_].getColumn()));
            }
        }
    }

    return std::make_unique<SpaceNode>(std::move(name), std::move(properties), std::move(shapes));
}

std::unique_ptr<IdentifierNode> Parser::parseIdentifier() {
    auto token = consume(Token::Type::Identifier, "Expected identifier");
    return std::make_unique<IdentifierNode>(
        token.getText().str(),
        Location(token.getLine(), token.getColumn()));
}

std::unique_ptr<PropertyNode> Parser::parseProperty() {
    consume(Token::Type::Property, "Expected property");
    auto name = parseIdentifier();
    consume(Token::Type::Colon, "Expected colon");
    auto type = parseType();
    consume(Token::Type::Equal, "Expected equal");
    auto value = parseExpression();
    consume(Token::Type::Semicolon, "Expected semicolon");
    return std::make_unique<PropertyNode>(std::move(name), std::move(type), std::move(value));
}

std::unique_ptr<ShapeNode> Parser::parseShape() {
    consume(Token::Type::Shape, "Expected shape");
    auto name = parseIdentifier();
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<MappingNode>> mappings;

    if (match(Token::Type::LeftBrace)) {
        while (!isAtEnd() && !match(Token::Type::RightBrace)) {
            if (match(Token::Type::Property)) {
                properties.push_back(parseProperty());
            } else if (match(Token::Type::Mapping)) {
                mappings.push_back(parseMapping());
            } else {
                throw ParseError("Expected property or mapping declaration",
                               Location(tokens_[current_].getLine(), tokens_[current_].getColumn()));
            }
        }
    }

    return std::make_unique<ShapeNode>(std::move(name), std::move(properties), std::move(mappings));
}

std::unique_ptr<MappingNode> Parser::parseMapping() {
    consume(Token::Type::Mapping, "Expected mapping");
    auto name = parseIdentifier();
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<ASTNode>> body;

    if (match(Token::Type::LeftBrace)) {
        while (!isAtEnd() && !match(Token::Type::RightBrace)) {
            if (match(Token::Type::Property)) {
                properties.push_back(parseProperty());
            } else {
                body.push_back(parseExpression());
            }
        }
    }

    return std::make_unique<MappingNode>(std::move(name), std::move(properties), std::move(body));
}

std::unique_ptr<TypeNode> Parser::parseType() {
    auto token = consume(Token::Type::Identifier, "Expected type");
    if (token.getText() == "Topology") {
        consume(Token::Type::LessThan, "Expected less than");
        auto baseType = parseType();
        consume(Token::Type::GreaterThan, "Expected greater than");
        return std::make_unique<TypeNode>(TypeNode::TypeKind::Topology, std::move(baseType));
    }
    return std::make_unique<TypeNode>(TypeNode::TypeKind::Basic);
}

std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    auto identifier = parseIdentifier();
    return std::unique_ptr<ExpressionNode>(new IdentifierExprNode(identifier->getName(), Location(identifier->getLocation())));
}

} // namespace topos::compiler
