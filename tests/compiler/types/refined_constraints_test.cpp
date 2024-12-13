#include <gtest/gtest.h>
#include "../../../src/compiler/types/refined_constraints.hpp"
#include "../../../src/compiler/types/type_system.hpp"
#include <memory>

namespace topos {
namespace types {
namespace test {

class RefinedConstraintsTest : public ::testing::Test {
protected:
    void SetUp() override {
        env_ = std::make_unique<TypeEnvironment>();
    }

    std::unique_ptr<TypeEnvironment> env_;
};

// トポロジカル制約の基本テスト
TEST_F(RefinedConstraintsTest, BasicTopologicalConstraintTest) {
    auto topology_type = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    auto constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(topology_type));
    
    EXPECT_TRUE(constraint->verify());
}

// 連結性検証のテスト
TEST_F(RefinedConstraintsTest, ConnectednessTest) {
    // 連結な位相空間
    auto connected_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    connected_topology->setProperty("connected", true);
    auto connected_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(connected_topology));
    EXPECT_TRUE(connected_constraint->verifyConnectedness());

    // 非連結な位相空間
    auto disconnected_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    disconnected_topology->setProperty("connected", false);
    auto disconnected_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(disconnected_topology));
    EXPECT_FALSE(disconnected_constraint->verifyConnectedness());
}

// 連続性検証のテスト
TEST_F(RefinedConstraintsTest, ContinuityTest) {
    // 連続な写像
    auto continuous_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    continuous_topology->setProperty("continuous", true);
    auto continuous_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(continuous_topology));
    EXPECT_TRUE(continuous_constraint->verifyContinuity());

    // 非連続な写像
    auto discontinuous_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    discontinuous_topology->setProperty("continuous", false);
    auto discontinuous_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(discontinuous_topology));
    EXPECT_FALSE(discontinuous_constraint->verifyContinuity());
}

// コンパクト性検証のテスト
TEST_F(RefinedConstraintsTest, CompactnessTest) {
    // コンパクトな空間
    auto compact_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    compact_topology->setProperty("compact", true);
    auto compact_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(compact_topology));
    EXPECT_TRUE(compact_constraint->verifyCompactness());

    // 非コンパクトな空間
    auto noncompact_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    noncompact_topology->setProperty("compact", false);
    auto noncompact_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(noncompact_topology));
    EXPECT_FALSE(noncompact_constraint->verifyCompactness());
}

// エラーケースのテスト
TEST_F(RefinedConstraintsTest, ErrorCaseTest) {
    // 無効な型でのテスト
    auto invalid_type = std::make_unique<BasicType>("invalid");
    auto invalid_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(invalid_type));
    EXPECT_FALSE(invalid_constraint->verify());
    
    // nullptrでのテスト
    EXPECT_THROW({
        auto null_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(nullptr);
    }, std::invalid_argument);
}

// 複合制約のテスト
TEST_F(RefinedConstraintsTest, CompositeConstraintTest) {
    RefinedConstraintSystem system;
    
    // 連結でコンパクトな位相空間
    auto topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    topology->setProperty("connected", true);
    topology->setProperty("compact", true);
    auto constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(topology));
    
    system.addConstraint(std::move(constraint));
    EXPECT_TRUE(system.verifyAll());
}

} // namespace test
} // namespace types
} // namespace topos
