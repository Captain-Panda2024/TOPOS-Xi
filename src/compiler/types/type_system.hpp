#ifndef TOPOS_COMPILER_TYPES_TYPE_SYSTEM_HPP
#define TOPOS_COMPILER_TYPES_TYPE_SYSTEM_HPP

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include "../ast/ast.hpp"
#include "../math/math_topology_verifier.hpp"
#include "../math/quantum_verifier.hpp"
#include "topology_traits.hpp"
#include "quantum_traits.hpp"

namespace topos {
namespace types {

// 型の基底クラス
class Type {
public:
    Type() = default;
    explicit Type(std::string name) : name_(std::move(name)) {}
    virtual ~Type() = default;
    virtual bool isSubtypeOf(const Type& other) const = 0;
    virtual std::string toString() const = 0;
    virtual bool verify() const = 0;

private:
    std::string name_;
};

// 基本型（int, float, bool など）
class BasicType : public Type {
public:
    explicit BasicType(const std::string& name) : Type(name), name_(name) {}

    bool isSubtypeOf(const Type& other) const override {
        if (auto basic = dynamic_cast<const BasicType*>(&other)) {
            return name_ == basic->name_;
        }
        return false;
    }

    std::string toString() const override { return name_; }
    bool verify() const override { return true; }

private:
    std::string name_;
};

// トポロジー型
class TopologyType : public Type {
public:
    explicit TopologyType(std::unique_ptr<Type> baseType)
        : Type("topology"), base_type_(std::move(baseType)), 
          topology_traits_(std::make_shared<ContinuousTopologyTraits>()) {
        if (!base_type_) {
            throw std::runtime_error("Base type is null");
        }
    }

    bool isSubtypeOf(const Type& other) const override {
        if (auto topology = dynamic_cast<const TopologyType*>(&other)) {
            return base_type_->isSubtypeOf(*topology->base_type_);
        }
        return false;
    }

    std::string toString() const override {
        return "Topology<" + base_type_->toString() + ">";
    }

    bool verify() const override {
        try {
            // 基本型の検証
            if (!base_type_->verify()) {
                lastError_ = "Base type verification failed";
                return false;
            }

            // 設定されたプロパティのみを検証
            for (const auto& prop : {
                TopologyTraits::Property::CONTINUOUS,
                TopologyTraits::Property::CONNECTED,
                TopologyTraits::Property::COMPACT
            }) {
                try {
                    if (!topology_traits_->verifyProperty(prop)) {
                        lastError_ = "Property verification failed: " + 
                                   TopologyTraits::propertyToString(prop);
                        return false;
                    }
                } catch (const std::exception& e) {
                    // プロパティが設定されていない場合はスキップ
                    continue;
                }
            }
            return true;
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }

    bool verifyProperty(TopologyTraits::Property prop) const {
        try {
            return topology_traits_->verifyProperty(prop);
        } catch (const std::exception& e) {
            lastError_ = std::string("Property verification failed: ") + e.what();
            return false;
        }
    }

    void setProperty(TopologyTraits::Property prop, bool value) {
        topology_traits_->setProperty(prop, value);
    }

    const Type* getBaseType() const { return base_type_.get(); }
    const TopologyTraits& getTopologyTraits() const { return *topology_traits_; }
    const std::string& getLastError() const { return lastError_; }

private:
    std::unique_ptr<Type> base_type_;
    std::shared_ptr<TopologyTraits> topology_traits_;
    mutable std::string lastError_;
};

// 量子型
class QuantumType : public Type {
public:
    explicit QuantumType(std::unique_ptr<Type> baseType)
        : Type("quantum"), base_type_(std::move(baseType)),
          quantum_traits_(std::make_shared<CoherentQuantumTraits>()) {
        if (!base_type_) {
            throw std::runtime_error("Base type is null");
        }
    }

    bool isSubtypeOf(const Type& other) const override {
        if (auto quantum = dynamic_cast<const QuantumType*>(&other)) {
            return base_type_->isSubtypeOf(*quantum->base_type_);
        }
        return false;
    }

    std::string toString() const override {
        return "Quantum<" + base_type_->toString() + ">";
    }

    bool verify() const override {
        try {
            return verifyProperty(QuantumTraits::Property::UNITARY) && 
                   verifyProperty(QuantumTraits::Property::NORMALIZED) && 
                   verifyProperty(QuantumTraits::Property::ENTANGLED) && 
                   base_type_->verify();
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }

    bool verifyProperty(QuantumTraits::Property prop) const {
        try {
            return quantum_traits_->verifyProperty(prop);
        } catch (const std::exception& e) {
            lastError_ = std::string("Property verification failed: ") + e.what();
            return false;
        }
    }

    void setProperty(QuantumTraits::Property prop, bool value) {
        quantum_traits_->setProperty(prop, value);
    }

    const Type* getBaseType() const { return base_type_.get(); }
    const QuantumTraits& getQuantumTraits() const { return *quantum_traits_; }
    const std::string& getLastError() const { return lastError_; }

private:
    std::unique_ptr<Type> base_type_;
    std::shared_ptr<QuantumTraits> quantum_traits_;
    mutable std::string lastError_;
};

// 型環境
class TypeEnvironment {
public:
    void addType(const std::string& name, std::unique_ptr<Type> type) {
        types_[name] = std::move(type);
    }

    const Type* lookupType(const std::string& name) const {
        auto it = types_.find(name);
        return it != types_.end() ? it->second.get() : nullptr;
    }

    bool verifyType(const std::string& name) const {
        auto type = lookupType(name);
        return type && type->verify();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Type>> types_;
};

// 型制約
class TypeConstraint {
public:
    enum class ConstraintKind {
        Subtype,    // サブタイプ制約
        Dependent,  // 依存型制約
        Continuous, // 連続性制約
        Quantum,    // 量子制約
        Custom      // カスタム制約
    };

    TypeConstraint(std::unique_ptr<Type> left,
                  std::unique_ptr<Type> right,
                  ConstraintKind kind)
        : left_(std::move(left))
        , right_(std::move(right))
        , kind_(kind) {
        if (!left_) {
            throw std::runtime_error("Left type is null");
        }
    }

    virtual ~TypeConstraint() = default;
    virtual bool verify() const = 0;

protected:
    const Type* getLeft() const { return left_.get(); }
    const Type* getRight() const { return right_.get(); }
    ConstraintKind getKind() const { return kind_; }

protected:
    std::unique_ptr<Type> left_;
    std::unique_ptr<Type> right_;
    ConstraintKind kind_;
};

// 型推論エンジン
class TypeInferenceEngine {
public:
    explicit TypeInferenceEngine(TypeEnvironment& env) : env_(env) {}

    std::optional<std::unique_ptr<Type>> infer(const ast::ASTNode& node) {
        // 型推論の実装
        return std::nullopt;
    }

    void addConstraint(std::unique_ptr<TypeConstraint> constraint) {
        constraints_.push_back(std::move(constraint));
    }

    bool verify() const {
        for (const auto& constraint : constraints_) {
            if (!constraint->verify()) {
                return false;
            }
        }
        return true;
    }

private:
    TypeEnvironment& env_;
    std::vector<std::unique_ptr<TypeConstraint>> constraints_;
};

} // namespace types
} // namespace topos

#endif // TOPOS_COMPILER_TYPES_TYPE_SYSTEM_HPP
