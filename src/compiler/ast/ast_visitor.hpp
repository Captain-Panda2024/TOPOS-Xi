#ifndef TOPOS_COMPILER_AST_AST_VISITOR_HPP
#define TOPOS_COMPILER_AST_AST_VISITOR_HPP

namespace topos {
namespace ast {

// Forward declarations
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
class IdentifierExprNode;
class NumberExprNode;
class StringExprNode;
class BinaryExprNode;
class UnaryExprNode;
class CallExprNode;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // 基本ノード
    virtual void visitIdentifier(IdentifierNode& node) = 0;
    virtual void visitType(TypeNode& node) = 0;
    virtual void visitProperty(PropertyNode& node) = 0;

    // 構造ノード
    virtual void visitMapping(MappingNode& node) = 0;
    virtual void visitShape(ShapeNode& node) = 0;
    virtual void visitSpace(SpaceNode& node) = 0;

    // パス関連ノード
    virtual void visitPathElement(PathElementNode& node) = 0;
    virtual void visitPath(PathNode& node) = 0;

    // 不変条件ノード
    virtual void visitInvariant(InvariantNode& node) = 0;

    // プログラムノード
    virtual void visitProgram(ProgramNode& node) = 0;

    // 式ノード
    virtual void visitIdentifierExpr(IdentifierExprNode& node) = 0;
    virtual void visitNumberExpr(NumberExprNode& node) = 0;
    virtual void visitStringExpr(StringExprNode& node) = 0;
    virtual void visitBinaryExpr(BinaryExprNode& node) = 0;
    virtual void visitUnaryExpr(UnaryExprNode& node) = 0;
    virtual void visitCallExpr(CallExprNode& node) = 0;
};

} // namespace ast
} // namespace topos

#endif // TOPOS_COMPILER_AST_AST_VISITOR_HPP
