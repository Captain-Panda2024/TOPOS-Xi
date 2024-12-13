#ifndef TOPOS_COMPILER_LEXER_LEXER_HPP
#define TOPOS_COMPILER_LEXER_LEXER_HPP

#include <string>
#include <vector>
#include "llvm/ADT/StringRef.h"

namespace topos {
namespace compiler {

class Token {
public:
    enum class Type {
        // キーワード
        Space,      // 'space'
        Shape,      // 'shape'
        Mapping,    // 'mapping'
        Property,   // 'property'
        Path,       // 'path'
        Invariant,  // 'invariant'

        // 識別子と文字列
        Identifier, // [a-zA-Z_][a-zA-Z0-9_]*
        String,     // "..."

        // 記号
        LeftBrace,    // {
        RightBrace,   // }
        LeftParen,    // (
        RightParen,   // )
        LessThan,     // <
        GreaterThan,  // >
        Equal,        // =
        Arrow,        // ->
        Colon,        // :
        Semicolon,    // ;
        Comma,        // ,

        // その他
        EndOfFile
    };

    Token(Type type, llvm::StringRef text, size_t line, size_t column)
        : type_(type), text_(text), line_(line), column_(column) {}

    Type getType() const { return type_; }
    llvm::StringRef getText() const { return text_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }

    Type type_;
    llvm::StringRef text_;
    size_t line_;
    size_t column_;
};

class Lexer {
public:
    explicit Lexer(llvm::StringRef input)
        : input_(input), current_(input.begin()),
          line_(1), column_(1) {}

    Token nextToken() {
        skipWhitespace();

        if (current_ >= input_.end()) {
            return Token(Token::Type::EndOfFile, "", line_, column_);
        }

        // 識別子またはキーワード
        if (isalpha(*current_) || *current_ == '_') {
            return lexIdentifierOrKeyword();
        }

        // 文字列リテラル
        if (*current_ == '"') {
            return lexString();
        }

        // 記号
        switch (*current_) {
            case '{': return lexSymbol(Token::Type::LeftBrace);
            case '}': return lexSymbol(Token::Type::RightBrace);
            case '(': return lexSymbol(Token::Type::LeftParen);
            case ')': return lexSymbol(Token::Type::RightParen);
            case '<': return lexSymbol(Token::Type::LessThan);
            case '>': return lexSymbol(Token::Type::GreaterThan);
            case '=': return lexSymbol(Token::Type::Equal);
            case ':': return lexSymbol(Token::Type::Colon);
            case ';': return lexSymbol(Token::Type::Semicolon);
            case ',': return lexSymbol(Token::Type::Comma);
            case '-':
                if (peek() == '>') {
                    return lexArrow();
                }
                // エラー処理
                break;
        }

        // エラー: 不正な文字
        return Token(Token::Type::EndOfFile, "", line_, column_);
    }

private:
    void skipWhitespace() {
        while (current_ < input_.end() && 
               (isspace(*current_) || *current_ == '\n')) {
            if (*current_ == '\n') {
                ++line_;
                column_ = 1;
            } else {
                ++column_;
            }
            ++current_;
        }
    }

    Token lexIdentifierOrKeyword() {
        const char* start = current_;
        size_t startColumn = column_;

        while (current_ < input_.end() && 
               (isalnum(*current_) || *current_ == '_')) {
            ++current_;
            ++column_;
        }

        llvm::StringRef text(start, current_ - start);
        Token::Type type = getKeywordType(text);

        return Token(type, text, line_, startColumn);
    }

    Token lexString() {
        const char* start = current_;
        size_t startColumn = column_;

        ++current_; // 開始の '"' をスキップ
        ++column_;

        while (current_ < input_.end() && *current_ != '"') {
            if (*current_ == '\\') {
                // エスケープシーケンスの処理
                ++current_;
                ++column_;
                if (current_ < input_.end()) {
                    ++current_;
                    ++column_;
                }
            } else {
                ++current_;
                ++column_;
            }
        }

        if (current_ < input_.end()) {
            ++current_; // 終了の '"' をスキップ
            ++column_;
        }

        return Token(Token::Type::String,
                    llvm::StringRef(start, current_ - start),
                    line_, startColumn);
    }

    Token lexSymbol(Token::Type type) {
        const char* start = current_;
        ++current_;
        ++column_;
        return Token(type, llvm::StringRef(start, 1), line_, column_ - 1);
    }

    Token lexArrow() {
        const char* start = current_;
        ++current_; // '-' をスキップ
        ++current_; // '>' をスキップ
        column_ += 2;
        return Token(Token::Type::Arrow,
                    llvm::StringRef(start, 2),
                    line_, column_ - 2);
    }

    char peek() const {
        if (current_ + 1 >= input_.end()) return '\0';
        return *(current_ + 1);
    }

    static Token::Type getKeywordType(llvm::StringRef text) {
        if (text == "space") return Token::Type::Space;
        if (text == "shape") return Token::Type::Shape;
        if (text == "mapping") return Token::Type::Mapping;
        if (text == "property") return Token::Type::Property;
        if (text == "path") return Token::Type::Path;
        if (text == "invariant") return Token::Type::Invariant;
        return Token::Type::Identifier;
    }

    llvm::StringRef input_;
    const char* current_;
    size_t line_;
    size_t column_;
};

} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_LEXER_LEXER_HPP
