#ifndef TOPOS_COMPILER_TYPES_REFINED_CONSTRAINTS_HPP
#define TOPOS_COMPILER_TYPES_REFINED_CONSTRAINTS_HPP

#include "type_system.hpp"
#include "dependent_type.hpp"
#include <vector>
#include <memory>

namespace topos {
namespace types {

// 拡張された型制約システム
class RefinedConstraintSystem {
public:
    // トポロジカル制約
    class TopologicalConstraint : public TypeConstraint {
    public:
        TopologicalConstraint(std::unique_ptr<Type> type)
            : TypeConstraint(std::move(type), nullptr, ConstraintKind::Continuous) {}

        bool verify() const override {
            return verifyConnectedness() && 
                   verifyContinuity() && 
                   verifyCompactness();
        }

    private:
        bool verifyConnectedness() const;
        bool verifyContinuity() const;
        bool verifyCompactness() const;
    };

    // 量子制約
    class QuantumConstraint : public TypeConstraint {
    public:
        QuantumConstraint(std::unique_ptr<Type> type)
            : TypeConstraint(std::move(type), nullptr, ConstraintKind::Quantum) {}

        bool verify() const override {
            return verifyUnitarity() && 
                   verifyNormalization() && 
                   verifyEntanglement();
        }

    private:
        bool verifyUnitarity() const;
        bool verifyNormalization() const;
        bool verifyEntanglement() const;
    };

    // 依存型制約
    class DependentConstraint : public TypeConstraint {
    public:
        DependentConstraint(std::unique_ptr<DependentType> type)
            : TypeConstraint(std::move(type), nullptr, ConstraintKind::Equal) {}

        bool verify() const override {
            if (auto dependent = dynamic_cast<const DependentType*>(left_.get())) {
                return dependent->verify() && dependent->verifyDependency(*right_);
            }
            return false;
        }
    };

    // 制約の追加と検証
    void addConstraint(std::unique_ptr<TypeConstraint> constraint) {
        constraints_.push_back(std::move(constraint));
    }

    bool verifyAll() const {
        for (const auto& constraint : constraints_) {
            if (!constraint->verify()) {
                return false;
            }
        }
        return true;
    }

private:
    std::vector<std::unique_ptr<TypeConstraint>> constraints_;
};

} // namespace types
} // namespace topos

#endif // TOPOS_COMPILER_TYPES_REFINED_CONSTRAINTS_HPP
