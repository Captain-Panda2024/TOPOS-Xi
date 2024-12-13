#ifndef TOPOS_COMPILER_TYPES_DEPENDENT_TYPE_HPP
#define TOPOS_COMPILER_TYPES_DEPENDENT_TYPE_HPP

#include "type_system.hpp"
#include "topology_traits.hpp"
#include "quantum_traits.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace topos {
namespace types {

// 依存型の拡張実装
class DependentType : public Type {
public:
    using Predicate = std::function<bool(const Type&)>;
    using TopologyConstraint = std::function<bool(const Type&)>;
    using QuantumConstraint = std::function<bool(const Type&)>;

    DependentType(
        std::unique_ptr<Type> baseType,
        Predicate pred,
        TopologyConstraint topoConstraint,
        QuantumConstraint quantumConstraint)
        : Type("dependent")
        , base_type_(std::move(baseType))
        , predicate_(std::move(pred))
        , topology_constraint_(std::move(topoConstraint))
        , quantum_constraint_(std::move(quantumConstraint)) {
        if (!base_type_) {
            throw std::runtime_error("Base type is null");
        }
        if (!predicate_) {
            throw std::runtime_error("Predicate is null");
        }
        if (!topology_constraint_) {
            throw std::runtime_error("Topology constraint is null");
        }
        if (!quantum_constraint_) {
            throw std::runtime_error("Quantum constraint is null");
        }
    }

    bool isSubtypeOf(const Type& other) const override {
        try {
            if (auto dependent = dynamic_cast<const DependentType*>(&other)) {
                return base_type_->isSubtypeOf(*dependent->base_type_) &&
                       predicate_(*dependent->base_type_) &&
                       verifyTopologicalConstraints(*dependent->base_type_) &&
                       verifyQuantumConstraints(*dependent->base_type_);
            }
            return false;
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }

    std::string toString() const override {
        try {
            return "Dependent<" + base_type_->toString() +
                   ", Topology=" + getTopologyString() +
                   ", Quantum=" + getQuantumString() + ">";
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return "Invalid dependent type";
        }
    }

    bool verify() const override {
        try {
            return base_type_->verify() &&
                   predicate_(*base_type_) &&
                   verifyTopologicalConstraints(*base_type_) &&
                   verifyQuantumConstraints(*base_type_);
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }

    const std::string& getLastError() const { return lastError_; }

private:
    bool verifyTopologicalConstraints(const Type& type) const {
        try {
            return topology_constraint_(type);
        } catch (const std::exception& e) {
            lastError_ = std::string("Topology constraint verification failed: ") + e.what();
            return false;
        }
    }

    bool verifyQuantumConstraints(const Type& type) const {
        try {
            return quantum_constraint_(type);
        } catch (const std::exception& e) {
            lastError_ = std::string("Quantum constraint verification failed: ") + e.what();
            return false;
        }
    }

    std::string getTopologyString() const {
        try {
            if (auto topology = dynamic_cast<const TopologyType*>(base_type_.get())) {
                return topology->toString();
            }
            return "none";
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return "invalid";
        }
    }

    std::string getQuantumString() const {
        try {
            if (auto quantum = dynamic_cast<const QuantumType*>(base_type_.get())) {
                return quantum->toString();
            }
            return "none";
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return "invalid";
        }
    }

    std::unique_ptr<Type> base_type_;
    Predicate predicate_;
    TopologyConstraint topology_constraint_;
    QuantumConstraint quantum_constraint_;
    mutable std::string lastError_;
};

// 拡張された依存型ビルダー
class DependentTypeBuilder {
public:
    static std::unique_ptr<DependentType> createRefinementType(
        std::unique_ptr<Type> baseType,
        DependentType::Predicate refinement,
        DependentType::TopologyConstraint topoConstraint,
        DependentType::QuantumConstraint quantumConstraint) {
        
        if (!baseType) {
            throw std::runtime_error("Base type is null");
        }
        if (!refinement) {
            throw std::runtime_error("Refinement is null");
        }
        if (!topoConstraint) {
            throw std::runtime_error("Topology constraint is null");
        }
        if (!quantumConstraint) {
            throw std::runtime_error("Quantum constraint is null");
        }

        return std::make_unique<DependentType>(
            std::move(baseType),
            std::move(refinement),
            std::move(topoConstraint),
            std::move(quantumConstraint)
        );
    }

    static std::unique_ptr<DependentType> createQuantumDependentType(
        std::unique_ptr<Type> baseType,
        DependentType::QuantumConstraint quantumConstraint) {
        
        if (!baseType) {
            throw std::runtime_error("Base type is null");
        }
        if (!quantumConstraint) {
            throw std::runtime_error("Quantum constraint is null");
        }

        return createRefinementType(
            std::move(baseType),
            [](const Type&) { return true; },
            [](const Type&) { return true; },
            std::move(quantumConstraint)
        );
    }

    static std::unique_ptr<DependentType> createTopologicalDependentType(
        std::unique_ptr<Type> baseType,
        DependentType::TopologyConstraint topoConstraint) {
        
        if (!baseType) {
            throw std::runtime_error("Base type is null");
        }
        if (!topoConstraint) {
            throw std::runtime_error("Topology constraint is null");
        }

        return createRefinementType(
            std::move(baseType),
            [](const Type&) { return true; },
            std::move(topoConstraint),
            [](const Type&) { return true; }
        );
    }

    static std::unique_ptr<DependentType> createCompositeDependentType(
        std::unique_ptr<Type> baseType,
        std::vector<DependentType::TopologyConstraint> topoConstraints,
        std::vector<DependentType::QuantumConstraint> quantumConstraints) {
        
        if (!baseType) {
            throw std::runtime_error("Base type is null");
        }

        return createRefinementType(
            std::move(baseType),
            [](const Type&) { return true; },
            [constraints = std::move(topoConstraints)](const Type& traits) {
                return std::all_of(constraints.begin(), constraints.end(),
                    [&traits](const auto& constraint) {
                        return constraint && constraint(traits);
                    });
            },
            [constraints = std::move(quantumConstraints)](const Type& traits) {
                return std::all_of(constraints.begin(), constraints.end(),
                    [&traits](const auto& constraint) {
                        return constraint && constraint(traits);
                    });
            }
        );
    }
};

} // namespace types
} // namespace topos

#endif // TOPOS_COMPILER_TYPES_DEPENDENT_TYPE_HPP
