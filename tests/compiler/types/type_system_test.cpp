#include <gtest/gtest.h>
#include "../../../src/compiler/types/type_system.hpp"
#include "../../../src/compiler/types/refined_constraints.hpp"
#include <complex>
#include <memory>

namespace topos {
namespace types {
namespace test {

class TypeSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        env_ = std::make_unique<TypeEnvironment>();
    }

    std::unique_ptr<TypeEnvironment> env_;
};

// 基本型のテスト
TEST_F(TypeSystemTest, BasicTypeTest) {
    auto int_type = std::make_unique<BasicType>("int");
    auto float_type = std::make_unique<BasicType>("float");
    
    EXPECT_TRUE(int_type->verify());
    EXPECT_TRUE(float_type->verify());
    EXPECT_FALSE(int_type->isSubtypeOf(*float_type));
    EXPECT_TRUE(int_type->isSubtypeOf(*int_type));
}

// トポロジカル型のテスト
TEST_F(TypeSystemTest, TopologyTypeTest) {
    auto base_type = std::make_unique<BasicType>("real");
    auto topology = std::make_unique<TopologyType>(std::move(base_type));
    
    EXPECT_TRUE(topology->verify());
    
    // 連結性のテスト
    auto connected_space = createConnectedSpace();
    EXPECT_TRUE(verifyConnectedness(*connected_space));
    
    // 分離性のテスト
    auto hausdorff_space = createHausdorffSpace();
    EXPECT_TRUE(verifySeparationAxioms(*hausdorff_space));
    
    // コンパクト性のテスト
    auto compact_space = createCompactSpace();
    EXPECT_TRUE(verifyCompactness(*compact_space));
}

// 量子型のテスト
TEST_F(TypeSystemTest, QuantumTypeTest) {
    auto base_type = std::make_unique<BasicType>("complex");
    auto quantum = std::make_unique<QuantumType>(std::move(base_type));
    
    EXPECT_TRUE(quantum->verify());
    
    // ユニタリ性のテスト
    auto unitary_gate = createUnitaryGate();
    EXPECT_TRUE(verifyUnitarity(*unitary_gate));
    
    // 正規化のテスト
    auto normalized_state = createNormalizedState();
    EXPECT_TRUE(verifyNormalization(*normalized_state));
    
    // 量子もつれのテスト
    auto entangled_state = createEntangledState();
    EXPECT_TRUE(verifyEntanglement(*entangled_state));
}

// 制約システムのテスト
TEST_F(TypeSystemTest, ConstraintSystemTest) {
    RefinedConstraintSystem constraints;
    
    // トポロジカル制約のテスト
    auto topology_type = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    auto topology_constraint = std::make_unique<TopologicalConstraint>(
        std::move(topology_type));
    constraints.addConstraint(std::move(topology_constraint));
    
    // 量子制約のテスト
    auto quantum_type = std::make_unique<QuantumType>(
        std::make_unique<BasicType>("complex"));
    auto quantum_constraint = std::make_unique<QuantumConstraint>(
        std::move(quantum_type));
    constraints.addConstraint(std::move(quantum_constraint));
    
    EXPECT_TRUE(constraints.verifyAll());
}

private:
    // テストヘルパー関数
    std::unique_ptr<TopologyType> createConnectedSpace() {
        auto base = std::make_unique<BasicType>("real");
        return std::make_unique<TopologyType>(std::move(base));
    }
    
    std::unique_ptr<TopologyType> createHausdorffSpace() {
        auto base = std::make_unique<BasicType>("real");
        return std::make_unique<TopologyType>(std::move(base));
    }
    
    std::unique_ptr<TopologyType> createCompactSpace() {
        auto base = std::make_unique<BasicType>("real");
        return std::make_unique<TopologyType>(std::move(base));
    }
    
    std::unique_ptr<QuantumType> createUnitaryGate() {
        auto base = std::make_unique<BasicType>("complex");
        return std::make_unique<QuantumType>(std::move(base));
    }
    
    std::unique_ptr<QuantumType> createNormalizedState() {
        auto base = std::make_unique<BasicType>("complex");
        return std::make_unique<QuantumType>(std::move(base));
    }
    
    std::unique_ptr<QuantumType> createEntangledState() {
        auto base = std::make_unique<BasicType>("complex");
        return std::make_unique<QuantumType>(std::move(base));
    }
};

} // namespace test
} // namespace types
} // namespace topos
