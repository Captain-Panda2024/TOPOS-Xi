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
    auto base_type = std::make_unique<BasicType>("qubit");
    auto quantum_type = std::make_unique<QuantumType>(std::move(base_type));
    
    EXPECT_TRUE(quantum_type->verify());
    
    // エンタングルメントのテスト
    auto entangled_state = createEntangledState();
    EXPECT_TRUE(verifyEntanglement(*entangled_state));
    
    // 重ね合わせのテスト
    auto superposition = createSuperposition();
    EXPECT_TRUE(verifySuperposition(*superposition));
    
    // 測定のテスト
    auto measurement = createMeasurement();
    EXPECT_TRUE(verifyMeasurement(*measurement));
}

// 制約システムのテスト
TEST_F(TypeSystemTest, ConstraintSystemTest) {
    auto constraint_system = std::make_unique<RefinedConstraintSystem>();
    auto type_a = std::make_unique<BasicType>("A");
    auto type_b = std::make_unique<BasicType>("B");
    
    // 制約の追加
    constraint_system->addConstraint(
        std::make_unique<SubtypeConstraint>(type_a.get(), type_b.get()));
    
    // 制約の検証
    EXPECT_TRUE(constraint_system->verifyAll());
    
    // 依存型の制約
    auto dependent_type = std::make_unique<DependentType>(
        std::move(type_a),
        [](const Type& t) { return true; }, // デフォルトの述語
        [](const Type& t) { return true; }, // デフォルトのトポロジー制約
        [](const Type& t) { return true; }  // デフォルトの量子制約
    );
    constraint_system->addConstraint(
        std::make_unique<DependentConstraint>(dependent_type.get(), type_b.get()));
    
    // 制約システム全体の検証
    EXPECT_TRUE(constraint_system->verifyAll());
}

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

std::unique_ptr<QuantumType> createSuperposition() {
    auto base = std::make_unique<BasicType>("complex");
    return std::make_unique<QuantumType>(std::move(base));
}

std::unique_ptr<QuantumType> createMeasurement() {
    auto base = std::make_unique<BasicType>("complex");
    return std::make_unique<QuantumType>(std::move(base));
}

} // namespace test
} // namespace types
} // namespace topos
