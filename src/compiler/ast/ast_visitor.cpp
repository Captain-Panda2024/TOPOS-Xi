#include "ast_visitor.hpp"
#include "ast.hpp"

namespace topos {
namespace ast {

void IdentifierNode::accept(ASTVisitor& visitor) { visitor.visitIdentifier(*this); }
void TypeNode::accept(ASTVisitor& visitor) { visitor.visitType(*this); }
void PropertyNode::accept(ASTVisitor& visitor) { visitor.visitProperty(*this); }
void MappingNode::accept(ASTVisitor& visitor) { visitor.visitMapping(*this); }
void ShapeNode::accept(ASTVisitor& visitor) { visitor.visitShape(*this); }
void SpaceNode::accept(ASTVisitor& visitor) { visitor.visitSpace(*this); }
void PathElementNode::accept(ASTVisitor& visitor) { visitor.visitPathElement(*this); }
void PathNode::accept(ASTVisitor& visitor) { visitor.visitPath(*this); }
void ProgramNode::accept(ASTVisitor& visitor) { visitor.visitProgram(*this); }
void InvariantNode::accept(ASTVisitor& visitor) { visitor.visitInvariant(*this); }
void IdentifierExprNode::accept(ASTVisitor& visitor) { visitor.visitIdentifierExpr(*this); }
void NumberExprNode::accept(ASTVisitor& visitor) { visitor.visitNumberExpr(*this); }
void StringExprNode::accept(ASTVisitor& visitor) { visitor.visitStringExpr(*this); }
void BinaryExprNode::accept(ASTVisitor& visitor) { visitor.visitBinaryExpr(*this); }
void UnaryExprNode::accept(ASTVisitor& visitor) { visitor.visitUnaryExpr(*this); }
void CallExprNode::accept(ASTVisitor& visitor) { visitor.visitCallExpr(*this); }

} // namespace ast
} // namespace topos
