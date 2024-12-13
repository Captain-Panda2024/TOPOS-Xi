#ifndef TOPOS_COMPILER_TYPES_REFINED_CONSTRAINTS_HPP
#define TOPOS_COMPILER_TYPES_REFINED_CONSTRAINTS_HPP

#include "type_system.hpp"
#include "dependent_type.hpp"
#include "topology_traits.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

namespace topos {
namespace types {

// 拡張された型制約システム
class RefinedConstraintSystem {
public:
    // トポロジカル制約の例外クラス
    class TopologicalConstraintError : public std::runtime_error {
    public:
        explicit TopologicalConstraintError(const std::string& message)
            : std::runtime_error(message) {}
    };

    // トポロジカル制約
    class TopologicalConstraint : public TypeConstraint {
    public:
        TopologicalConstraint(std::unique_ptr<Type> type)
            : TypeConstraint(std::move(type), nullptr, ConstraintKind::Continuous) {
            if (!left_) {
                throw TopologicalConstraintError("Type is null");
            }
        }

        bool verify() const override {
            try {
                auto topology = dynamic_cast<const TopologyType*>(left_.get());
                if (!topology) {
                    throw TopologicalConstraintError("Type is not a topology type");
                }
                return topology->verify();
            } catch (const std::exception& e) {
                lastError_ = e.what();
                return false;
            }
        }

        bool verifyConnectedness() const {
            try {
                auto topology = dynamic_cast<const TopologyType*>(left_.get());
                if (!topology) {
                    throw TopologicalConstraintError("Type is not a topology type");
                }
                return topology->verifyProperty(TopologyTraits::Property::CONNECTED);
            } catch (const std::exception& e) {
                lastError_ = std::string("Connectedness verification failed: ") + e.what();
                return false;
            }
        }

        bool verifyContinuity() const {
            try {
                auto topology = dynamic_cast<const TopologyType*>(left_.get());
                if (!topology) {
                    throw TopologicalConstraintError("Type is not a topology type");
                }
                return topology->verifyProperty(TopologyTraits::Property::CONTINUOUS);
            } catch (const std::exception& e) {
                lastError_ = std::string("Continuity verification failed: ") + e.what();
                return false;
            }
        }

        bool verifyCompactness() const {
            try {
                auto topology = dynamic_cast<const TopologyType*>(left_.get());
                if (!topology) {
                    throw TopologicalConstraintError("Type is not a topology type");
                }
                return topology->verifyProperty(TopologyTraits::Property::COMPACT);
            } catch (const std::exception& e) {
                lastError_ = std::string("Compactness verification failed: ") + e.what();
                return false;
            }
        }

        const std::string& getLastError() const { return lastError_; }

    private:
        mutable std::string lastError_;
    };

    // サブタイプ制約
    class SubtypeConstraint : public TypeConstraint {
    public:
        SubtypeConstraint(std::unique_ptr<Type> left, std::unique_ptr<Type> right)
            : TypeConstraint(std::move(left), std::move(right), ConstraintKind::Subtype) {
            if (!left_ || !right_) {
                throw std::runtime_error("Left or right type is null");
            }
        }

        bool verify() const override {
            try {
                return left_->isSubtypeOf(*right_);
            } catch (const std::exception& e) {
                lastError_ = e.what();
                return false;
            }
        }

        const std::string& getLastError() const { return lastError_; }

    private:
        mutable std::string lastError_;
    };

    // 依存型制約
    class DependentConstraint : public TypeConstraint {
    public:
        DependentConstraint(std::unique_ptr<Type> type, std::unique_ptr<DependentType> depType)
            : TypeConstraint(std::move(type), nullptr, ConstraintKind::Dependent),
              dependentType_(std::move(depType)) {
            if (!left_ || !dependentType_) {
                throw std::runtime_error("Type or dependent type is null");
            }
        }

        bool verify() const override {
            try {
                return dependentType_->verify(*left_);
            } catch (const std::exception& e) {
                lastError_ = e.what();
                return false;
            }
        }

        const std::string& getLastError() const { return lastError_; }

    private:
        std::unique_ptr<DependentType> dependentType_;
        mutable std::string lastError_;
    };

    // 制約を追加
    void addConstraint(std::unique_ptr<TypeConstraint> constraint) {
        if (!constraint) {
            throw std::runtime_error("Constraint is null");
        }
        constraints_.push_back(std::move(constraint));
    }

    // すべての制約を検証
    bool verifyAll() const {
        try {
            for (const auto& constraint : constraints_) {
                if (!constraint->verify()) {
                    if (auto topological = dynamic_cast<const TopologicalConstraint*>(constraint.get())) {
                        lastError_ = topological->getLastError();
                    }
                    return false;
                }
            }
            return true;
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }

    const std::string& getLastError() const { return lastError_; }

private:
    std::vector<std::unique_ptr<TypeConstraint>> constraints_;
    mutable std::string lastError_;
};

} // namespace types
} // namespace topos

#endif // TOPOS_COMPILER_TYPES_REFINED_CONSTRAINTS_HPP
