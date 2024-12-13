#include <gtest/gtest.h>
#include "../../../src/compiler/types/refined_constraints.hpp"
#include "../../../src/compiler/types/type_system.hpp"
#include "../../../src/compiler/types/topology_traits.hpp"
#include <memory>
#include <stdexcept>

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
    connected_topology->setProperty(TopologyTraits::Property::CONNECTED, true);
    auto connected_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(connected_topology));
    EXPECT_TRUE(connected_constraint->verifyConnectedness());

    // 非連結な位相空間
    auto disconnected_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    disconnected_topology->setProperty(TopologyTraits::Property::CONNECTED, false);
    auto disconnected_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(disconnected_topology));
    EXPECT_FALSE(disconnected_constraint->verifyConnectedness());
}

// 連続性検証のテスト
TEST_F(RefinedConstraintsTest, ContinuityTest) {
    // 連続な写像
    auto continuous_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    continuous_topology->setProperty(TopologyTraits::Property::CONTINUOUS, true);
    auto continuous_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(continuous_topology));
    EXPECT_TRUE(continuous_constraint->verifyContinuity());

    // 非連続な写像
    auto discontinuous_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    discontinuous_topology->setProperty(TopologyTraits::Property::CONTINUOUS, false);
    auto discontinuous_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(discontinuous_topology));
    EXPECT_FALSE(discontinuous_constraint->verifyContinuity());
}

// コンパクト性検証のテスト
TEST_F(RefinedConstraintsTest, CompactnessTest) {
    // コンパクトな位相空間
    auto compact_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    compact_topology->setProperty(TopologyTraits::Property::COMPACT, true);
    auto compact_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(compact_topology));
    EXPECT_TRUE(compact_constraint->verifyCompactness());

    // 非コンパクトな位相空間
    auto noncompact_topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    noncompact_topology->setProperty(TopologyTraits::Property::COMPACT, false);
    auto noncompact_constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
        std::move(noncompact_topology));
    EXPECT_FALSE(noncompact_constraint->verifyCompactness());
}

// エラーケースのテスト
TEST_F(RefinedConstraintsTest, ErrorCaseTest) {
    // nullポインタのテスト
    EXPECT_THROW({
        auto constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(nullptr);
    }, RefinedConstraintSystem::TopologicalConstraintError);

    // 無効な型のテスト
    auto invalid_type = std::make_unique<BasicType>("invalid");
    EXPECT_THROW({
        auto constraint = std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
            std::move(invalid_type));
        constraint->verify();
    }, RefinedConstraintSystem::TopologicalConstraintError);
}

// 複合制約のテスト
TEST_F(RefinedConstraintsTest, CompositeConstraintTest) {
    RefinedConstraintSystem system;

    // トポロジー型を作成
    auto topology = std::make_unique<TopologyType>(
        std::make_unique<BasicType>("real"));
    topology->setProperty(TopologyTraits::Property::CONNECTED, true);
    topology->setProperty(TopologyTraits::Property::CONTINUOUS, true);
    topology->setProperty(TopologyTraits::Property::COMPACT, true);

    // 制約を追加
    system.addConstraint(
        std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
            std::move(topology)));

    // すべての制約を検証
    EXPECT_TRUE(system.verifyAll());
}

} // namespace test
} // namespace types
} // namespace topos
