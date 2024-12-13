#ifndef TOPOS_COMPILER_AST_AST_HPP
#define TOPOS_COMPILER_AST_AST_HPP

#include <memory>
#include <vector>
#include <string>
#include "llvm/ADT/StringRef.h"
#include "ast_visitor.hpp"  // ASTVisitorクラスの定義をインクルード

namespace topos {
namespace ast {

// Forward declarations
class ASTVisitor;
class IdentifierNode;
class TypeNode;
class PropertyNode;
class MappingNode;
class ShapeNode;
class SpaceNode;
class PathElementNode;
class PathNode;
class ProgramNode;
class InvariantNode;
class ExpressionNode;

// 基本的なASTノード
class ASTNode {
public:
    virtual ~ASTNode() = 0; // 純粋仮想デストラクタ
    virtual void accept(ASTVisitor& visitor) = 0;
};

// 位置情報
struct Location {
    size_t line;
    size_t column;
    
    Location(size_t l, size_t c) : line(l), column(c) {}
};

// 識別子ノード
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const std::string& name, Location loc)
        : name_(name), location_(loc) {}

    virtual ~IdentifierNode() override;
    const std::string& getName() const { return name_; }
    Location getLocation() const { return location_; }

    void accept(ASTVisitor& visitor) override;

private:
    std::string name_;
    Location location_;
};

// 型ノード
class TypeNode : public ASTNode {
public:
    enum class TypeKind {
        Basic,      // 基本型
        Topology,   // トポロジー型
        Quantum,    // 量子型
        Custom      // カスタム型
    };

    TypeNode(TypeKind kind, std::unique_ptr<TypeNode> base = nullptr)
        : kind_(kind), base_type_(std::move(base)) {}

    virtual ~TypeNode() override;
    TypeKind getKind() const { return kind_; }
    TypeNode* getBaseType() const { return base_type_.get(); }

    void accept(ASTVisitor& visitor) override;

private:
    TypeKind kind_;
    std::unique_ptr<TypeNode> base_type_;
};

// プロパティノード
class PropertyNode : public ASTNode {
public:
    PropertyNode(std::unique_ptr<IdentifierNode> name,
                std::unique_ptr<TypeNode> type,
                std::unique_ptr<ASTNode> value = nullptr)
        : name_(std::move(name)), 
          type_(std::move(type)),
          value_(std::move(value)) {}

    virtual ~PropertyNode() override;
    const IdentifierNode* getName() const { return name_.get(); }
    const TypeNode* getType() const { return type_.get(); }
    const ASTNode* getValue() const { return value_.get(); }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<IdentifierNode> name_;
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ASTNode> value_;
};

// マッピングノード
class MappingNode : public ASTNode {
public:
    MappingNode(std::unique_ptr<IdentifierNode> name,
                std::vector<std::unique_ptr<PropertyNode>> properties,
                std::vector<std::unique_ptr<ASTNode>> body)
        : name_(std::move(name)),
          properties_(std::move(properties)),
          body_(std::move(body)) {}

    virtual ~MappingNode() override;
    const IdentifierNode* getName() const { return name_.get(); }
    const std::vector<std::unique_ptr<PropertyNode>>& getProperties() const {
        return properties_;
    }
    const std::vector<std::unique_ptr<ASTNode>>& getBody() const {
        return body_;
    }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<IdentifierNode> name_;
    std::vector<std::unique_ptr<PropertyNode>> properties_;
    std::vector<std::unique_ptr<ASTNode>> body_;
};

// シェイプノード
class ShapeNode : public ASTNode {
public:
    ShapeNode(std::unique_ptr<IdentifierNode> name,
              std::vector<std::unique_ptr<PropertyNode>> properties,
              std::vector<std::unique_ptr<MappingNode>> mappings)
        : name_(std::move(name)),
          properties_(std::move(properties)),
          mappings_(std::move(mappings)) {}

    virtual ~ShapeNode() override;
    const IdentifierNode* getName() const { return name_.get(); }
    const std::vector<std::unique_ptr<PropertyNode>>& getProperties() const {
        return properties_;
    }
    const std::vector<std::unique_ptr<MappingNode>>& getMappings() const {
        return mappings_;
    }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<IdentifierNode> name_;
    std::vector<std::unique_ptr<PropertyNode>> properties_;
    std::vector<std::unique_ptr<MappingNode>> mappings_;
};

// スペースノード
class SpaceNode : public ASTNode {
public:
    SpaceNode(std::unique_ptr<IdentifierNode> name,
              std::vector<std::unique_ptr<PropertyNode>> properties,
              std::vector<std::unique_ptr<ShapeNode>> shapes)
        : name_(std::move(name)),
          properties_(std::move(properties)),
          shapes_(std::move(shapes)) {}

    virtual ~SpaceNode() override;
    const IdentifierNode* getName() const { return name_.get(); }
    const std::vector<std::unique_ptr<PropertyNode>>& getProperties() const {
        return properties_;
    }
    const std::vector<std::unique_ptr<ShapeNode>>& getShapes() const {
        return shapes_;
    }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<IdentifierNode> name_;
    std::vector<std::unique_ptr<PropertyNode>> properties_;
    std::vector<std::unique_ptr<ShapeNode>> shapes_;
};

// パス要素ノード
class PathElementNode : public ASTNode {
public:
    PathElementNode(std::unique_ptr<IdentifierNode> name)
        : name_(std::move(name)) {}

    virtual ~PathElementNode() override;
    const IdentifierNode* getName() const { return name_.get(); }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<IdentifierNode> name_;
};

// パスノード
class PathNode : public ASTNode {
public:
    PathNode(std::vector<std::unique_ptr<PathElementNode>> elements)
        : elements_(std::move(elements)) {}

    virtual ~PathNode() override;
    const std::vector<std::unique_ptr<PathElementNode>>& getElements() const {
        return elements_;
    }

    void accept(ASTVisitor& visitor) override;

private:
    std::vector<std::unique_ptr<PathElementNode>> elements_;
};

// 式ノード
class ExpressionNode : public ASTNode {
public:
    enum class ExprKind {
        Identifier,
        Number,
        String,
        Binary,
        Unary,
        Call
    };

    ExpressionNode(ExprKind kind, Location loc)
        : kind_(kind), location_(loc) {}

    virtual ~ExpressionNode() override;
    ExprKind getKind() const { return kind_; }
    Location getLocation() const { return location_; }

    virtual void accept(ASTVisitor& visitor) = 0;

protected:
    ExprKind kind_;
    Location location_;
};

// 識別子式ノード
class IdentifierExprNode : public ExpressionNode {
public:
    IdentifierExprNode(const std::string& name, Location loc)
        : ExpressionNode(ExprKind::Identifier, loc), name_(name) {}

    virtual ~IdentifierExprNode() override;
    const std::string& getName() const { return name_; }

    void accept(ASTVisitor& visitor) override;

private:
    std::string name_;
};

// 数値式ノード
class NumberExprNode : public ExpressionNode {
public:
    NumberExprNode(double value, Location loc)
        : ExpressionNode(ExprKind::Number, loc), value_(value) {}

    virtual ~NumberExprNode() override;
    double getValue() const { return value_; }

    void accept(ASTVisitor& visitor) override;

private:
    double value_;
};

// 文字列式ノード
class StringExprNode : public ExpressionNode {
public:
    StringExprNode(const std::string& value, Location loc)
        : ExpressionNode(ExprKind::String, loc), value_(value) {}

    virtual ~StringExprNode() override;
    const std::string& getValue() const { return value_; }

    void accept(ASTVisitor& visitor) override;

private:
    std::string value_;
};

// 二項演算式ノード
class BinaryExprNode : public ExpressionNode {
public:
    enum class OpKind {
        Add,     // +
        Sub,     // -
        Mul,     // *
        Div,     // /
        Equal,   // ==
        NotEq,   // !=
        Less,    // <
        LessEq,  // <=
        Greater, // >
        GreatEq  // >=
    };

    BinaryExprNode(std::unique_ptr<ExpressionNode> left,
                  std::unique_ptr<ExpressionNode> right,
                  OpKind op,
                  Location loc)
        : ExpressionNode(ExprKind::Binary, loc),
          left_(std::move(left)),
          right_(std::move(right)),
          op_(op) {}

    virtual ~BinaryExprNode() override;
    const ExpressionNode* getLeft() const { return left_.get(); }
    const ExpressionNode* getRight() const { return right_.get(); }
    OpKind getOp() const { return op_; }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<ExpressionNode> left_;
    std::unique_ptr<ExpressionNode> right_;
    OpKind op_;
};

// 単項演算式ノード
class UnaryExprNode : public ExpressionNode {
public:
    enum class OpKind {
        Neg,  // -
        Not   // !
    };

    UnaryExprNode(std::unique_ptr<ExpressionNode> operand,
                  OpKind op,
                  Location loc)
        : ExpressionNode(ExprKind::Unary, loc),
          operand_(std::move(operand)),
          op_(op) {}

    virtual ~UnaryExprNode() override;
    const ExpressionNode* getOperand() const { return operand_.get(); }
    OpKind getOp() const { return op_; }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<ExpressionNode> operand_;
    OpKind op_;
};

// 関数呼び出し式ノード
class CallExprNode : public ExpressionNode {
public:
    CallExprNode(std::unique_ptr<ExpressionNode> callee,
                 std::vector<std::unique_ptr<ExpressionNode>> args,
                 Location loc)
        : ExpressionNode(ExprKind::Call, loc),
          callee_(std::move(callee)),
          args_(std::move(args)) {}

    virtual ~CallExprNode() override;
    const ExpressionNode* getCallee() const { return callee_.get(); }
    const std::vector<std::unique_ptr<ExpressionNode>>& getArgs() const { return args_; }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<ExpressionNode> callee_;
    std::vector<std::unique_ptr<ExpressionNode>> args_;
};

// 不変条件ノード
class InvariantNode : public ASTNode {
public:
    InvariantNode(std::unique_ptr<IdentifierNode> name,
                 std::unique_ptr<TypeNode> type,
                 std::unique_ptr<ASTNode> condition)
        : name_(std::move(name)),
          type_(std::move(type)),
          condition_(std::move(condition)) {}

    virtual ~InvariantNode() override;
    const IdentifierNode* getName() const { return name_.get(); }
    const TypeNode* getType() const { return type_.get(); }
    const ASTNode* getCondition() const { return condition_.get(); }

    void accept(ASTVisitor& visitor) override;

private:
    std::unique_ptr<IdentifierNode> name_;
    std::unique_ptr<TypeNode> type_;
    std::unique_ptr<ASTNode> condition_;
};

// プログラムノード
class ProgramNode : public ASTNode {
public:
    ProgramNode(std::vector<std::unique_ptr<SpaceNode>> spaces)
        : spaces_(std::move(spaces)) {}

    virtual ~ProgramNode() override;
    const std::vector<std::unique_ptr<SpaceNode>>& getSpaces() const {
        return spaces_;
    }

    const std::vector<std::unique_ptr<SpaceNode>>& getNodes() const {
        return spaces_;
    }

    void accept(ASTVisitor& visitor) override;

private:
    std::vector<std::unique_ptr<SpaceNode>> spaces_;
};

// AST構築用のビルダークラス
class ASTBuilder {
public:
    std::unique_ptr<ProgramNode> buildProgram(
        std::vector<std::unique_ptr<SpaceNode>> spaces) {
        return std::make_unique<ProgramNode>(std::move(spaces));
    }

    std::unique_ptr<SpaceNode> buildSpace(
        std::unique_ptr<IdentifierNode> name,
        std::vector<std::unique_ptr<PropertyNode>> properties,
        std::vector<std::unique_ptr<ShapeNode>> shapes) {
        return std::make_unique<SpaceNode>(
            std::move(name),
            std::move(properties),
            std::move(shapes));
    }

    std::unique_ptr<ShapeNode> buildShape(
        std::unique_ptr<IdentifierNode> name,
        std::vector<std::unique_ptr<PropertyNode>> properties,
        std::vector<std::unique_ptr<MappingNode>> mappings) {
        return std::make_unique<ShapeNode>(
            std::move(name),
            std::move(properties),
            std::move(mappings));
    }

    std::unique_ptr<MappingNode> buildMapping(
        std::unique_ptr<IdentifierNode> name,
        std::vector<std::unique_ptr<PropertyNode>> properties,
        std::vector<std::unique_ptr<ASTNode>> body) {
        return std::make_unique<MappingNode>(
            std::move(name),
            std::move(properties),
            std::move(body));
    }

    std::unique_ptr<PropertyNode> buildProperty(
        std::unique_ptr<IdentifierNode> name,
        std::unique_ptr<TypeNode> type,
        std::unique_ptr<ASTNode> value = nullptr) {
        return std::make_unique<PropertyNode>(
            std::move(name),
            std::move(type),
            std::move(value));
    }

    std::unique_ptr<TypeNode> buildType(
        TypeNode::TypeKind kind,
        std::unique_ptr<TypeNode> base = nullptr) {
        return std::make_unique<TypeNode>(kind, std::move(base));
    }

    std::unique_ptr<IdentifierNode> buildIdentifier(
        const std::string& name,
        Location location) {
        return std::make_unique<IdentifierNode>(name, location);
    }

    std::unique_ptr<PathNode> buildPath(
        std::vector<std::unique_ptr<PathElementNode>> elements) {
        return std::make_unique<PathNode>(std::move(elements));
    }

    std::unique_ptr<PathElementNode> buildPathElement(
        std::unique_ptr<IdentifierNode> name) {
        return std::make_unique<PathElementNode>(std::move(name));
    }

    std::unique_ptr<InvariantNode> buildInvariant(
        std::unique_ptr<IdentifierNode> name,
        std::unique_ptr<TypeNode> type,
        std::unique_ptr<ASTNode> condition) {
        return std::make_unique<InvariantNode>(
            std::move(name),
            std::move(type),
            std::move(condition));
    }

    std::unique_ptr<ExpressionNode> buildIdentifierExpr(
        const std::string& name,
        Location location) {
        return std::make_unique<IdentifierExprNode>(name, location);
    }

    std::unique_ptr<ExpressionNode> buildNumberExpr(
        double value,
        Location location) {
        return std::make_unique<NumberExprNode>(value, location);
    }

    std::unique_ptr<ExpressionNode> buildStringExpr(
        const std::string& value,
        Location location) {
        return std::make_unique<StringExprNode>(value, location);
    }

    std::unique_ptr<ExpressionNode> buildBinaryExpr(
        std::unique_ptr<ExpressionNode> left,
        std::unique_ptr<ExpressionNode> right,
        BinaryExprNode::OpKind op,
        Location location) {
        return std::make_unique<BinaryExprNode>(
            std::move(left),
            std::move(right),
            op,
            location);
    }

    std::unique_ptr<ExpressionNode> buildUnaryExpr(
        std::unique_ptr<ExpressionNode> operand,
        UnaryExprNode::OpKind op,
        Location location) {
        return std::make_unique<UnaryExprNode>(
            std::move(operand),
            op,
            location);
    }

    std::unique_ptr<ExpressionNode> buildCallExpr(
        std::unique_ptr<ExpressionNode> callee,
        std::vector<std::unique_ptr<ExpressionNode>> args,
        Location location) {
        return std::make_unique<CallExprNode>(
            std::move(callee),
            std::move(args),
            location);
    }
};

} // namespace ast
} // namespace topos

#endif // TOPOS_COMPILER_AST_AST_HPP
