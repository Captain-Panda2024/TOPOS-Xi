#include <gtest/gtest.h>
#include "topology/transform/continuous_transform.hpp"
#include "topology/space/topological_space.hpp"
#include "compiler/types/basic_type.hpp"

namespace topos {
namespace topology {
namespace test {

class ContinuousTransformTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 基本的な実数型を作成
        auto real_type = std::make_unique<types::BasicType>("real");
        
        // 実数直線上の標準的な位相空間を作成
        domain_ = std::make_shared<space::TopologicalSpace<double>>(
            std::move(real_type)
        );
        
        // 基本的な開集合を追加
        domain_->addOpenSet({{-1.0, 1.0}}); // 区間(-1,1)
        
        // 値域も同様に設定
        codomain_ = domain_;
    }

    // 2倍にする連続関数
    static double double_function(const double& x) {
        return 2.0 * x;
    }

    // 正弦関数
    static double sine_function(const double& x) {
        return std::sin(x);
    }

    std::shared_ptr<space::TopologicalSpace<double>> domain_;
    std::shared_ptr<space::TopologicalSpace<double>> codomain_;
};

TEST_F(ContinuousTransformTest, BasicTransformation) {
    // 2倍にする変換を作成
    transform::ContinuousTransform<double> double_transform(
        domain_,
        codomain_,
        double_function
    );

    // 基本的な変換のテスト
    EXPECT_DOUBLE_EQ(double_transform.apply(1.0), 2.0);
    EXPECT_DOUBLE_EQ(double_transform.apply(-1.0), -2.0);
    EXPECT_DOUBLE_EQ(double_transform.apply(0.0), 0.0);
}

TEST_F(ContinuousTransformTest, Composition) {
    // 2倍とサイン関数の変換を作成
    transform::ContinuousTransform<double> double_transform(
        domain_,
        codomain_,
        double_function
    );

    transform::ContinuousTransform<double> sine_transform(
        domain_,
        codomain_,
        sine_function
    );

    // 合成関数を作成
    auto composed = double_transform.compose(sine_transform);
    ASSERT_NE(composed, nullptr);

    // 合成関数のテスト
    EXPECT_DOUBLE_EQ(composed->apply(0.0), 0.0);
    EXPECT_NEAR(composed->apply(M_PI/2), 2.0, 1e-10);
}

TEST_F(ContinuousTransformTest, Continuity) {
    // 連続関数のテスト
    transform::ContinuousTransform<double> sine_transform(
        domain_,
        codomain_,
        sine_function
    );
    EXPECT_TRUE(sine_transform.verifyContinuity());

    // 不連続関数のテスト（TODO: 実装後に追加）
}

} // namespace test
} // namespace topology
} // namespace topos
