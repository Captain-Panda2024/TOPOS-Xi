#ifndef TOPOS_COMPILER_QUANTUM_SYNTAX_HPP
#define TOPOS_COMPILER_QUANTUM_SYNTAX_HPP

#include "../types/quantum_traits.hpp"
#include "../types/topology_traits.hpp"
#include <string>
#include <memory>
#include <vector>

namespace topos {
namespace compiler {
namespace quantum {

// 量子構文要素の基底クラス
class QuantumSyntaxNode {
public:
    virtual ~QuantumSyntaxNode() = default;
    virtual std::string toString() const = 0;
};

// 量子状態宣言
class QuantumStateDeclaration : public QuantumSyntaxNode {
public:
    QuantumStateDeclaration(
        const std::string& name,
        const std::vector<std::string>& type_params)
        : name_(name), type_parameters_(type_params) {}

    std::string toString() const override {
        std::string result = "quantum " + name_ + "<";
        for (size_t i = 0; i < type_parameters_.size(); ++i) {
            if (i > 0) result += ", ";
            result += type_parameters_[i];
        }
        result += ">";
        return result;
    }

private:
    std::string name_;
    std::vector<std::string> type_parameters_;
};

// 量子演算子宣言
class QuantumOperatorDeclaration : public QuantumSyntaxNode {
public:
    enum class OperatorKind {
        Unitary,
        Observable,
        Measurement
    };

    QuantumOperatorDeclaration(
        const std::string& name,
        OperatorKind kind,
        const std::vector<std::string>& params)
        : name_(name), kind_(kind), parameters_(params) {}

    std::string toString() const override {
        std::string result = getKindString() + " " + name_ + "(";
        for (size_t i = 0; i < parameters_.size(); ++i) {
            if (i > 0) result += ", ";
            result += parameters_[i];
        }
        result += ")";
        return result;
    }

private:
    std::string name_;
    OperatorKind kind_;
    std::vector<std::string> parameters_;

    std::string getKindString() const {
        switch (kind_) {
            case OperatorKind::Unitary: return "unitary";
            case OperatorKind::Observable: return "observable";
            case OperatorKind::Measurement: return "measure";
            default: return "unknown";
        }
    }
};

// 位相的性質の指定
class TopologicalPropertyDeclaration : public QuantumSyntaxNode {
public:
    enum class PropertyKind {
        Continuous,
        Connected,
        Compact
    };

    TopologicalPropertyDeclaration(
        const std::string& target,
        PropertyKind kind)
        : target_(target), kind_(kind) {}

    std::string toString() const override {
        return "topology " + target_ + " is " + getPropertyString();
    }

private:
    std::string target_;
    PropertyKind kind_;

    std::string getPropertyString() const {
        switch (kind_) {
            case PropertyKind::Continuous: return "continuous";
            case PropertyKind::Connected: return "connected";
            case PropertyKind::Compact: return "compact";
            default: return "unknown";
        }
    }
};

// 量子操作の構文
class QuantumOperation : public QuantumSyntaxNode {
public:
    enum class OperationType {
        Apply,      // 演算子の適用
        Transform,  // 状態変換
        Compose     // 演算子の合成
    };

    QuantumOperation(
        OperationType type,
        const std::string& target,
        const std::string& operator_name)
        : type_(type), target_(target), operator_(operator_name) {}

    std::string toString() const override {
        switch (type_) {
            case OperationType::Apply:
                return operator_ + " " + target_;
            case OperationType::Transform:
                return "transform " + target_ + " by " + operator_;
            case OperationType::Compose:
                return "compose " + target_ + " with " + operator_;
            default:
                return "unknown operation";
        }
    }

private:
    OperationType type_;
    std::string target_;
    std::string operator_;
};

} // namespace quantum
} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_QUANTUM_SYNTAX_HPP
