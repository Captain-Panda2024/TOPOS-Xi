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
        bool verifyConnectedness() const {
            if (auto topology = dynamic_cast<const TopologyType*>(left_.get())) {
                return topology->verifyProperty("connected");
            }
            return false;
        }

        bool verifyContinuity() const {
            if (auto topology = dynamic_cast<const TopologyType*>(left_.get())) {
                return topology->verifyProperty("continuous");
            }
            return false;
        }

        bool verifyCompactness() const {
            if (auto topology = dynamic_cast<const TopologyType*>(left_.get())) {
                return topology->verifyProperty("compact");
            }
            return false;
        }
    };

    // サブタイプ制約
    class SubtypeConstraint : public TypeConstraint {
    public:
        SubtypeConstraint(Type* left, Type* right)
            : TypeConstraint(nullptr, nullptr, ConstraintKind::Subtype) {
            left_ = left;
            right_ = right;
        }

        bool verify() const override {
            return left_->isSubtypeOf(*right_);
        }
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
        bool verifyUnitarity() const {
            if (auto quantum = dynamic_cast<const QuantumType*>(left_.get())) {
                return quantum->verifyProperty("unitary");
            }
            return false;
        }

        bool verifyNormalization() const {
            if (auto quantum = dynamic_cast<const QuantumType*>(left_.get())) {
                return quantum->verifyProperty("normalized");
            }
            return false;
        }

        bool verifyEntanglement() const {
            if (auto quantum = dynamic_cast<const QuantumType*>(left_.get())) {
                return quantum->verifyProperty("entangled");
            }
            return false;
        }
    };

    // 依存型制約
    class DependentConstraint : public TypeConstraint {
    public:
        DependentConstraint(std::unique_ptr<DependentType> type)
            : TypeConstraint(std::move(type), nullptr, ConstraintKind::Equal) {}

        bool verify() const override {
            if (auto dependent = dynamic_cast<const DependentType*>(left_.get())) {
                return dependent->verify() && dependent->isSubtypeOf(*right_);
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
