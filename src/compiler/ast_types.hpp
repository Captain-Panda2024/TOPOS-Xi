
// TOPOS-Ξ AST Type Nodes
namespace topos {
namespace ast {

class TypeNode : public ASTNode {
    std::unique_ptr<types::Type> type_info;

public:
    explicit TypeNode(std::unique_ptr<types::Type> type)
        : type_info(std::move(type)) {}

    NodeType getType() const override { return NodeType::Type; }
    
    bool verify() const {
        return type_info && type_info->verify_continuity() && 
               type_info->verify_preservation();
    }
};

class TopologyNode : public TypeNode {
    std::vector<std::unique_ptr<ASTNode>> constraints;

public:
    template<typename T>
    explicit TopologyNode(std::unique_ptr<types::Topology<T>> topology)
        : TypeNode(std::move(topology)) {}

    void add_constraint(std::unique_ptr<ASTNode> constraint) {
        constraints.push_back(std::move(constraint));
    }
};

class QuantumNode : public TypeNode {
    std::vector<std::unique_ptr<ASTNode>> superposition_states;

public:
    template<typename T>
    explicit QuantumNode(std::unique_ptr<types::Quantum<T>> quantum)
        : TypeNode(std::move(quantum)) {}

    void add_state(std::unique_ptr<ASTNode> state) {
        superposition_states.push_back(std::move(state));
    }
};

// 型検証のためのビジター
class TypeVerificationVisitor {
    types::TypeChecker checker;

public:
    bool visit(const TypeNode& node) {
        return node.verify();
    }

    bool visit(const TopologyNode& node) {
        return node.verify();
    }

    bool visit(const QuantumNode& node) {
        return node.verify();
    }
};

} // namespace ast
} // namespace topos