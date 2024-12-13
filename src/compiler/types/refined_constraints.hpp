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
        SubtypeConstraint(const Type* subtype, const Type* supertype)
            : TypeConstraint(
                std::unique_ptr<Type>(const_cast<Type*>(subtype)), 
                std::unique_ptr<Type>(const_cast<Type*>(supertype)), 
                ConstraintKind::Subtype) {}

        bool verify() const override {
            return getLeft() && getRight() && getLeft()->isSubtypeOf(*getRight());
        }
    };

    class SubtypeConstraint2 : public TypeConstraint {
    public:
        SubtypeConstraint2(const Type* subtype, const Type* supertype)
            : subtype_(subtype), supertype_(supertype) {}

        bool verify() const override {
            return subtype_ && supertype_ && subtype_->isSubtypeOf(*supertype_);
        }

    private:
        const Type* subtype_;
        const Type* supertype_;
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
        DependentConstraint(const Type* dependent, const Type* target)
            : TypeConstraint(
                std::unique_ptr<Type>(const_cast<Type*>(dependent)), 
                std::unique_ptr<Type>(const_cast<Type*>(target)), 
                ConstraintKind::Dependent) {}

        bool verify() const override {
            return getLeft() && getRight() && getLeft()->isSubtypeOf(*getRight());
        }
    };

    class DependentConstraint2 : public TypeConstraint {
    public:
        DependentConstraint2(const Type* dependent, const Type* target)
            : dependent_(dependent), target_(target) {}

        bool verify() const override {
            return dependent_ && target_ && dependent_->isSubtypeOf(*target_);
        }

    private:
        const Type* dependent_;
        const Type* target_;
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
