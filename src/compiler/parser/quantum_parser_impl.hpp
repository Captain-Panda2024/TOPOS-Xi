#ifndef TOPOS_COMPILER_PARSER_QUANTUM_PARSER_IMPL_HPP
#define TOPOS_COMPILER_PARSER_QUANTUM_PARSER_IMPL_HPP

#include "quantum_parser.hpp"
#include "../lexer/token.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

namespace topos {
namespace compiler {
namespace parser {

class QuantumParserImpl : public QuantumParser {
public:
    explicit QuantumParserImpl(std::vector<Token> tokens)
        : tokens_(std::move(tokens)), current_(0) {}

private:
    std::vector<Token> tokens_;
    size_t current_;

    // キーワードセット
    static const std::unordered_set<std::string> quantum_keywords_;
    static const std::unordered_set<std::string> operator_keywords_;
    static const std::unordered_set<std::string> topology_keywords_;

    // トークン操作
    Token peek() const {
        if (isAtEnd()) return Token{TokenType::EOF_TOKEN};
        return tokens_[current_];
    }

    Token advance() {
        if (!isAtEnd()) current_++;
        return previous();
    }

    Token previous() const {
        return tokens_[current_ - 1];
    }

    bool isAtEnd() const {
        return current_ >= tokens_.size();
    }

    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    // パーサーヘルパー実装
    std::optional<std::string> parseIdentifier() override {
        if (match(TokenType::IDENTIFIER)) {
            return previous().lexeme;
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>> parseTypeParameters() override {
        if (!match(TokenType::LESS)) {
            return std::nullopt;
        }

        std::vector<std::string> params;
        do {
            auto param = parseIdentifier();
            if (!param) {
                return std::nullopt;
            }
            params.push_back(*param);
        } while (match(TokenType::COMMA));

        if (!match(TokenType::GREATER)) {
            return std::nullopt;
        }

        return params;
    }

    std::optional<quantum::QuantumOperatorDeclaration::OperatorKind>
    parseOperatorKind() override {
        if (match(TokenType::KEYWORD)) {
            const auto& keyword = previous().lexeme;
            if (keyword == "unitary") {
                return quantum::QuantumOperatorDeclaration::OperatorKind::Unitary;
            }
            if (keyword == "observable") {
                return quantum::QuantumOperatorDeclaration::OperatorKind::Observable;
            }
            if (keyword == "measure") {
                return quantum::QuantumOperatorDeclaration::OperatorKind::Measurement;
            }
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string>> parseParameters() override {
        if (!match(TokenType::LEFT_PAREN)) {
            return std::nullopt;
        }

        std::vector<std::string> params;
        if (!match(TokenType::RIGHT_PAREN)) {
            do {
                auto param = parseIdentifier();
                if (!param) {
                    return std::nullopt;
                }
                params.push_back(*param);
            } while (match(TokenType::COMMA));

            if (!match(TokenType::RIGHT_PAREN)) {
                return std::nullopt;
            }
        }

        return params;
    }

    std::optional<quantum::TopologicalPropertyDeclaration::PropertyKind>
    parsePropertyKind() override {
        if (match(TokenType::KEYWORD)) {
            const auto& keyword = previous().lexeme;
            if (keyword == "continuous") {
                return quantum::TopologicalPropertyDeclaration::PropertyKind::Continuous;
            }
            if (keyword == "connected") {
                return quantum::TopologicalPropertyDeclaration::PropertyKind::Connected;
            }
            if (keyword == "compact") {
                return quantum::TopologicalPropertyDeclaration::PropertyKind::Compact;
            }
        }
        return std::nullopt;
    }

    std::optional<quantum::QuantumOperation::OperationType>
    parseOperationType() override {
        if (match(TokenType::KEYWORD)) {
            const auto& keyword = previous().lexeme;
            if (keyword == "apply") {
                return quantum::QuantumOperation::OperationType::Apply;
            }
            if (keyword == "transform") {
                return quantum::QuantumOperation::OperationType::Transform;
            }
            if (keyword == "compose") {
                return quantum::QuantumOperation::OperationType::Compose;
            }
        }
        return std::nullopt;
    }

    bool expectKeyword(const std::string& keyword) override {
        if (match(TokenType::KEYWORD) && previous().lexeme == keyword) {
            return true;
        }
        return false;
    }
};

// キーワードセットの初期化
const std::unordered_set<std::string> QuantumParserImpl::quantum_keywords_ = {
    "quantum", "state", "register"
};

const std::unordered_set<std::string> QuantumParserImpl::operator_keywords_ = {
    "unitary", "observable", "measure"
};

const std::unordered_set<std::string> QuantumParserImpl::topology_keywords_ = {
    "topology", "continuous", "connected", "compact"
};

} // namespace parser
} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_PARSER_QUANTUM_PARSER_IMPL_HPP
