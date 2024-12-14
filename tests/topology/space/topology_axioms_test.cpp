#include <gtest/gtest.h>
#include "topology/space/topology_axioms.hpp"
#include "topology/space/topological_space.hpp"
#include "compiler/types/basic_type.hpp"

namespace topos {
namespace topology {
namespace test {

class TopologyAxiomsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 実数区間[-1, 1]を全体集合として設定
        universe_ = {-1.0, -0.5, 0.0, 0.5, 1.0};  // 離散的な表現

        // 基本的な開集合を準備
        empty_set_ = {};
        left_half_ = {-1.0, -0.5, 0.0};
        right_half_ = {0.0, 0.5, 1.0};
        middle_ = {-0.5, 0.0, 0.5};
    }

    std::set<double> universe_;
    std::set<double> empty_set_;
    std::set<double> left_half_;
    std::set<double> right_half_;
    std::set<double> middle_;
};

TEST_F(TopologyAxiomsTest, EmptyAndWholeSpace) {
    std::vector<std::set<double>> open_sets;
    
    // 空集合と全体集合のみ
    open_sets = {empty_set_, universe_};
    EXPECT_TRUE(space::TopologyAxioms<double>::verifyEmptyAndWholeSpace(
        open_sets, universe_));

    // 空集合がない場合
    open_sets = {universe_, left_half_};
    EXPECT_FALSE(space::TopologyAxioms<double>::verifyEmptyAndWholeSpace(
        open_sets, universe_));

    // 全体集合がない場合
    open_sets = {empty_set_, left_half_};
    EXPECT_FALSE(space::TopologyAxioms<double>::verifyEmptyAndWholeSpace(
        open_sets, universe_));
}

TEST_F(TopologyAxiomsTest, FiniteIntersection) {
    std::vector<std::set<double>> open_sets;
    
    // 基本的な開集合の族
    open_sets = {empty_set_, universe_, left_half_, right_half_};
    
    // 交わりを計算
    std::set<double> intersection;
    std::set_intersection(
        left_half_.begin(), left_half_.end(),
        right_half_.begin(), right_half_.end(),
        std::inserter(intersection, intersection.begin())
    );
    
    // 交わりを追加
    open_sets.push_back(intersection);
    
    EXPECT_TRUE(space::TopologyAxioms<double>::verifyFiniteIntersection(open_sets));
}

TEST_F(TopologyAxiomsTest, ArbitraryUnion) {
    std::vector<std::set<double>> open_sets;
    
    // 基本的な開集合の族
    open_sets = {empty_set_, universe_, left_half_, right_half_};
    
    // 全体集合を設定
    space::TopologyAxioms<double>::universe_ = universe_;
    
    EXPECT_TRUE(space::TopologyAxioms<double>::verifyArbitraryUnion(open_sets));
    
    // 和集合が開集合でない場合を追加
    std::set<double> non_open = {-1.0, 1.0};  // 端点のみ
    open_sets.push_back(non_open);
    
    EXPECT_FALSE(space::TopologyAxioms<double>::verifyArbitraryUnion(open_sets));
}

TEST_F(TopologyAxiomsTest, TopologicalSpaceIntegration) {
    // 位相空間を作成
    auto real_type = std::make_unique<types::BasicType>("real");
    space::TopologicalSpace<double> space(std::move(real_type), universe_);
    
    // 開集合を追加
    EXPECT_TRUE(space.addOpenSet(left_half_));
    EXPECT_TRUE(space.addOpenSet(right_half_));
    EXPECT_TRUE(space.addOpenSet(middle_));
    
    // 位相公理を満たすことを確認
    EXPECT_TRUE(space.verifyTopologyAxioms());
    
    // 無効な開集合の追加を試みる
    std::set<double> invalid_set = {-1.0, 1.0};  // 端点のみ
    EXPECT_FALSE(space.addOpenSet(invalid_set));
    
    // 位相公理は依然として満たされていることを確認
    EXPECT_TRUE(space.verifyTopologyAxioms());
}

} // namespace test
} // namespace topology
} // namespace topos
