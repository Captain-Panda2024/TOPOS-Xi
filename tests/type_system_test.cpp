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
