#include "ast.hpp"

namespace topos {
namespace ast {

// デストラクタの実装
ASTNode::~ASTNode() = default;
ProgramNode::~ProgramNode() = default;
SpaceNode::~SpaceNode() = default;
ShapeNode::~ShapeNode() = default;
MappingNode::~MappingNode() = default;
PropertyNode::~PropertyNode() = default;
TypeNode::~TypeNode() = default;
IdentifierNode::~IdentifierNode() = default;
PathElementNode::~PathElementNode() = default;
PathNode::~PathNode() = default;
InvariantNode::~InvariantNode() = default;

// 式ノードの実装
ExpressionNode::~ExpressionNode() = default;
IdentifierExprNode::~IdentifierExprNode() = default;
NumberExprNode::~NumberExprNode() = default;
StringExprNode::~StringExprNode() = default;
BinaryExprNode::~BinaryExprNode() = default;
UnaryExprNode::~UnaryExprNode() = default;
CallExprNode::~CallExprNode() = default;

// accept() メソッドの実装
void ProgramNode::accept(ASTVisitor& visitor) {
    visitor.visitProgram(*this);
}

void SpaceNode::accept(ASTVisitor& visitor) {
    visitor.visitSpace(*this);
}

void ShapeNode::accept(ASTVisitor& visitor) {
    visitor.visitShape(*this);
}

void MappingNode::accept(ASTVisitor& visitor) {
    visitor.visitMapping(*this);
}

void PropertyNode::accept(ASTVisitor& visitor) {
    visitor.visitProperty(*this);
}

void TypeNode::accept(ASTVisitor& visitor) {
    visitor.visitType(*this);
}

void IdentifierNode::accept(ASTVisitor& visitor) {
    visitor.visitIdentifier(*this);
}

void PathElementNode::accept(ASTVisitor& visitor) {
    visitor.visitPathElement(*this);
}

void PathNode::accept(ASTVisitor& visitor) {
    visitor.visitPath(*this);
}

void InvariantNode::accept(ASTVisitor& visitor) {
    visitor.visitInvariant(*this);
}

void IdentifierExprNode::accept(ASTVisitor& visitor) {
    visitor.visitIdentifierExpr(*this);
}

void NumberExprNode::accept(ASTVisitor& visitor) {
    visitor.visitNumberExpr(*this);
}

void StringExprNode::accept(ASTVisitor& visitor) {
    visitor.visitStringExpr(*this);
}

void BinaryExprNode::accept(ASTVisitor& visitor) {
    visitor.visitBinaryExpr(*this);
}

void UnaryExprNode::accept(ASTVisitor& visitor) {
    visitor.visitUnaryExpr(*this);
}

void CallExprNode::accept(ASTVisitor& visitor) {
    visitor.visitCallExpr(*this);
}

} // namespace ast
} // namespace topos
