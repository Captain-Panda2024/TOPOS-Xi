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
};

// Implement accept methods for each node type
inline void IdentifierNode::accept(ASTVisitor& visitor) { visitor.visitIdentifier(*this); }
inline void TypeNode::accept(ASTVisitor& visitor) { visitor.visitType(*this); }
inline void PropertyNode::accept(ASTVisitor& visitor) { visitor.visitProperty(*this); }
inline void MappingNode::accept(ASTVisitor& visitor) { visitor.visitMapping(*this); }
inline void ShapeNode::accept(ASTVisitor& visitor) { visitor.visitShape(*this); }
inline void SpaceNode::accept(ASTVisitor& visitor) { visitor.visitSpace(*this); }
inline void PathElementNode::accept(ASTVisitor& visitor) { visitor.visitPathElement(*this); }
inline void PathNode::accept(ASTVisitor& visitor) { visitor.visitPath(*this); }
inline void ProgramNode::accept(ASTVisitor& visitor) { visitor.visitProgram(*this); }
inline void InvariantNode::accept(ASTVisitor& visitor) { visitor.visitInvariant(*this); }

} // namespace ast
} // namespace topos

#endif // TOPOS_COMPILER_AST_AST_VISITOR_HPP
