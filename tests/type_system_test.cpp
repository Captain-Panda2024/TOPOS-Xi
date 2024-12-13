#include <gtest/gtest.h>
#include "compiler/types/type_system.hpp"

using namespace topos::compiler;

TEST(TypeSystemTest, BasicTypeChecking) {
    TypeSystem typeSystem;

    // 基本型のテスト
    auto intType = typeSystem.createBasicType("Int");
    EXPECT_NE(intType, nullptr);

    // トポロジカル型のテスト
    auto pointType = typeSystem.createTopologyType("Point");
    EXPECT_NE(pointType, nullptr);

    // 量子型のテスト
    auto qubitType = typeSystem.createQuantumType("Qubit", 1);
    EXPECT_NE(qubitType, nullptr);

    // サブタイプ関係のテスト
    auto result = typeSystem.isSubtype(intType, intType);
    EXPECT_TRUE(result);
}

TEST(TopologyTypeTest, BasicProperties) {
    using namespace topos::types;

    // 基本型の作成
    auto numberType = std::make_unique<BasicType>("Number");
    EXPECT_TRUE(numberType->verify());

    // トポロジカル型の作成
    auto topologyType = std::make_unique<TopologyType>(std::move(numberType));
    
    // 基本的な性質の検証
    EXPECT_TRUE(topologyType->verify());
    EXPECT_TRUE(topologyType->verifyProperty("connected"));
    EXPECT_TRUE(topologyType->verifyProperty("continuous"));
    EXPECT_TRUE(topologyType->verifyProperty("compact"));
}

TEST(TopologyTypeTest, SeparationAxioms) {
    using namespace topos::types;
    
    // 離散トポロジーのテスト（Boolean型）
    auto boolType = std::make_unique<BasicType>("Boolean");
    auto discreteTopology = std::make_unique<TopologyType>(std::move(boolType));
    
    // 分離公理の検証
    EXPECT_TRUE(discreteTopology->verifyConnectedness());
    EXPECT_TRUE(discreteTopology->verifySeparationAxioms());
}

TEST(TopologyTypeTest, CompoundTypes) {
    using namespace topos::types;
    
    // 基本型からの複合型の構築
    auto numberType = std::make_unique<BasicType>("Number");
    auto baseTopology = std::make_unique<TopologyType>(std::move(numberType));
    auto compoundTopology = std::make_unique<TopologyType>(std::move(baseTopology));
    
    // 複合型の性質検証
    EXPECT_TRUE(compoundTopology->verify());
    EXPECT_TRUE(compoundTopology->verifyConnectedness());
    EXPECT_TRUE(compoundTopology->verifyCompactness());
}
