#ifndef TOPOS_QUANTUM_TYPE_INTEGRATION_HPP
#define TOPOS_QUANTUM_TYPE_INTEGRATION_HPP

#include "../compiler/types/dependent_type.hpp"
#include "topology.hpp"
#include "circuit.hpp"
#include "operators.hpp"
#include <memory>

namespace topos {
namespace quantum {

// 量子状態の型
class QuantumStateType : public types::Type {
public:
    explicit QuantumStateType(std::shared_ptr<QubitState> state)
        : state_(state),
          topology_(std::make_unique<QuantumStateTopology>(*state)) {}

    bool isSubtypeOf(const Type& other) const override {
        if (auto quantum_type = dynamic_cast<const QuantumStateType*>(&other)) {
            return verifyStateCompatibility(*quantum_type);
        }
        return false;
    }

    std::string toString() const override {
        return "QuantumState<" + getStateDescription() + ">";
    }

    bool verify() const override {
        return topology_->verifyProperty(
            types::TopologyTraits::Property::Continuous) &&
               topology_->verifyProperty(
            types::TopologyTraits::Property::Connected);
    }

    // 量子的性質の取得
    std::shared_ptr<types::QuantumTraits> getQuantumTraits() const {
        return std::make_shared<StateQuantumTraits>(*state_);
    }

    // 位相的性質の取得
    std::shared_ptr<types::TopologyTraits> getTopologyTraits() const {
        return std::make_shared<QuantumStateTopology>(*state_);
    }

private:
    std::shared_ptr<QubitState> state_;
    std::unique_ptr<QuantumStateTopology> topology_;

    bool verifyStateCompatibility(const QuantumStateType& other) const {
        return topology_->verifyMapping(
            types::TopologyTraits::MappingType::Homeomorphism,
            *other.topology_);
    }

    std::string getStateDescription() const {
        const auto& state_vector = state_->getState();
        if (state_vector.size() != 2) return "Invalid";
        
        // 基底状態の判定
        if (std::abs(state_vector[0]) > 0.99) return "|0⟩";
        if (std::abs(state_vector[1]) > 0.99) return "|1⟩";
        return "Superposition";
    }
};

// 量子回路の型
class QuantumCircuitType : public types::Type {
public:
    explicit QuantumCircuitType(std::shared_ptr<QuantumCircuit> circuit)
        : circuit_(circuit),
          topology_(std::make_unique<QuantumCircuitTopology>(*circuit)) {}

    bool isSubtypeOf(const Type& other) const override {
        if (auto circuit_type = dynamic_cast<const QuantumCircuitType*>(&other)) {
            return verifyCircuitCompatibility(*circuit_type);
        }
        return false;
    }

    std::string toString() const override {
        return "QuantumCircuit<" + getCircuitDescription() + ">";
    }

    bool verify() const override {
        return topology_->verifyProperty(
            types::TopologyTraits::Property::Continuous) &&
               circuit_->verifyQuantumProperties();
    }

    // 量子的性質の取得
    std::shared_ptr<types::QuantumTraits> getQuantumTraits() const {
        return std::make_shared<CircuitQuantumTraits>(*circuit_);
    }

    // 位相的性質の取得
    std::shared_ptr<types::TopologyTraits> getTopologyTraits() const {
        return std::make_shared<QuantumCircuitTopology>(*circuit_);
    }

private:
    std::shared_ptr<QuantumCircuit> circuit_;
    std::unique_ptr<QuantumCircuitTopology> topology_;

    bool verifyCircuitCompatibility(const QuantumCircuitType& other) const {
        return topology_->verifyMapping(
            types::TopologyTraits::MappingType::Homotopy,
            *other.topology_);
    }

    std::string getCircuitDescription() const {
        return "Unitary";  // 簡略化された実装
    }
};

// 量子演算子の型
class QuantumOperatorType : public types::Type {
public:
    explicit QuantumOperatorType(std::shared_ptr<QuantumOperator> op)
        : operator_(op) {}

    bool isSubtypeOf(const Type& other) const override {
        if (auto op_type = dynamic_cast<const QuantumOperatorType*>(&other)) {
            return verifyOperatorCompatibility(*op_type);
        }
        return false;
    }

    std::string toString() const override {
        return "QuantumOperator<" + getOperatorDescription() + ">";
    }

    bool verify() const override {
        return operator_->isUnitary() || operator_->isHermitian();
    }

private:
    std::shared_ptr<QuantumOperator> operator_;

    bool verifyOperatorCompatibility(const QuantumOperatorType& other) const {
        // 演算子の互換性検証
        return true;  // 実装は省略
    }

    std::string getOperatorDescription() const {
        if (operator_->isUnitary()) return "Unitary";
        if (operator_->isHermitian()) return "Hermitian";
        return "General";
    }
};

// 型生成ユーティリティ
class QuantumTypeFactory {
public:
    static std::unique_ptr<types::DependentType> createQuantumStateType(
        std::shared_ptr<QubitState> state) {
        auto base_type = std::make_unique<QuantumStateType>(state);
        return types::DependentTypeBuilder::createCompositeDependentType(
            std::move(base_type),
            {&verifyQuantumTopology},
            {&verifyQuantumCoherence}
        );
    }

    static std::unique_ptr<types::DependentType> createQuantumCircuitType(
        std::shared_ptr<QuantumCircuit> circuit) {
        auto base_type = std::make_unique<QuantumCircuitType>(circuit);
        return types::DependentTypeBuilder::createCompositeDependentType(
            std::move(base_type),
            {&verifyCircuitTopology},
            {&verifyCircuitQuantum}
        );
    }

private:
    static bool verifyQuantumTopology(const types::TopologyTraits& traits) {
        return traits.verifyProperty(
            types::TopologyTraits::Property::Continuous);
    }

    static bool verifyQuantumCoherence(const types::QuantumTraits& traits) {
        return traits.verifyProperty(
            types::QuantumTraits::Property::Coherent);
    }

    static bool verifyCircuitTopology(const types::TopologyTraits& traits) {
        return traits.verifyProperty(
            types::TopologyTraits::Property::Connected);
    }

    static bool verifyCircuitQuantum(const types::QuantumTraits& traits) {
        return traits.verifyProperty(
            types::QuantumTraits::Property::Unitary);
    }
};

} // namespace quantum
} // namespace topos

#endif // TOPOS_QUANTUM_TYPE_INTEGRATION_HPP
