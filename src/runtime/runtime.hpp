#ifndef TOPOS_RUNTIME_RUNTIME_HPP
#define TOPOS_RUNTIME_RUNTIME_HPP

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include "../compiler/types/type_system.hpp"
#include "../compiler/quantum/gates.hpp"

namespace topos {
namespace runtime {

// 実行時の値を表現するクラス
class Value {
public:
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
    virtual const compiler::Type* getType() const = 0;
};

// 基本型の値
class BasicValue : public Value {
public:
    explicit BasicValue(const compiler::BasicType* type) : type_(type) {}
    const compiler::Type* getType() const override { return type_; }

protected:
    const compiler::BasicType* type_;
};

// トポロジカル型の値
class TopologyValue : public Value {
public:
    explicit TopologyValue(const compiler::TopologyType* type) : type_(type) {}
    const compiler::Type* getType() const override { return type_; }

protected:
    const compiler::TopologyType* type_;
};

// 量子型の値
class QuantumValue : public Value {
public:
    explicit QuantumValue(const compiler::QuantumType* type)
        : type_(type), state_(1 << type->getNumQubits(), 0.0) {
        // 初期状態を |0...0⟩ に設定
        state_[0] = std::complex<double>(1.0, 0.0);
    }

    const compiler::Type* getType() const override { return type_; }

    void applyGate(const quantum::QuantumGate& gate) {
        gate.apply(state_);
    }

    std::string toString() const override {
        std::string result = "|ψ⟩ = ";
        for (size_t i = 0; i < state_.size(); ++i) {
            if (std::abs(state_[i]) > 1e-10) {
                if (i > 0 && state_[i].real() >= 0) {
                    result += "+";
                }
                result += "(" + std::to_string(state_[i].real()) + "+" +
                         std::to_string(state_[i].imag()) + "i)|" +
                         std::bitset<8>(i).to_string().substr(8 - type_->getNumQubits()) + "⟩";
            }
        }
        return result;
    }

private:
    const compiler::QuantumType* type_;
    std::vector<std::complex<double>> state_;
};

// 実行時環境
class RuntimeEnvironment {
public:
    void setValue(const std::string& name, std::unique_ptr<Value> value) {
        values_[name] = std::move(value);
    }

    Value* getValue(const std::string& name) const {
        auto it = values_.find(name);
        return it != values_.end() ? it->second.get() : nullptr;
    }

    void clear() {
        values_.clear();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Value>> values_;
};

// 実行時システム
class RuntimeSystem {
public:
    explicit RuntimeSystem(std::shared_ptr<compiler::TypeSystem> typeSystem)
        : typeSystem_(std::move(typeSystem)) {}

    RuntimeEnvironment& getEnvironment() { return env_; }

    // 型チェックと値の作成
    std::unique_ptr<Value> createValue(const compiler::Type* type) {
        if (auto basicType = dynamic_cast<const compiler::BasicType*>(type)) {
            return std::make_unique<BasicValue>(basicType);
        }
        if (auto topologyType = dynamic_cast<const compiler::TopologyType*>(type)) {
            return std::make_unique<TopologyValue>(topologyType);
        }
        if (auto quantumType = dynamic_cast<const compiler::QuantumType*>(type)) {
            return std::make_unique<QuantumValue>(quantumType);
        }
        return nullptr;
    }

    // 型システムへのアクセス
    const compiler::TypeSystem& getTypeSystem() const { return *typeSystem_; }

private:
    std::shared_ptr<compiler::TypeSystem> typeSystem_;
    RuntimeEnvironment env_;
};

} // namespace runtime
} // namespace topos

#endif // TOPOS_RUNTIME_RUNTIME_HPP
