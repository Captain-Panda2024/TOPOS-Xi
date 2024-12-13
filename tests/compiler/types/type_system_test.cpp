#include <gtest/gtest.h>
#include "compiler/types/type_system.hpp"
#include "compiler/types/refined_constraints.hpp"
#include "compiler/types/dependent_type.hpp"
#include "compiler/types/topology_traits.hpp"
#include "compiler/types/quantum_traits.hpp"
#include <memory>

namespace topos {
namespace types {
namespace test {

// Forward declarations of helper functions
std::unique_ptr<TopologyType> createConnectedSpace();
std::unique_ptr<TopologyType> createHausdorffSpace();
std::unique_ptr<TopologyType> createCompactSpace();
std::unique_ptr<QuantumType> createEntangledState();
std::unique_ptr<QuantumType> createSuperposition();
std::unique_ptr<QuantumType> createMeasurement();

bool verifyConnectedness(const TopologyType& space);
bool verifySeparationAxioms(const TopologyType& space);
bool verifyCompactness(const TopologyType& space);
bool verifyEntanglement(const QuantumType& state);
bool verifySuperposition(const QuantumType& state);
bool verifyMeasurement(const QuantumType& state);

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

// サブタイプ制約のテスト
TEST_F(TypeSystemTest, SubtypeConstraintTest) {
    try {
        // 基本型の作成
        auto float_type = std::make_unique<BasicType>("float");
        auto number_type = std::make_unique<BasicType>("number");

        // 型の検証
        EXPECT_TRUE(float_type->verify()) << "Float type verification failed";
        EXPECT_TRUE(number_type->verify()) << "Number type verification failed";

        // サブタイプ関係の検証
        EXPECT_TRUE(float_type->isSubtypeOf(*number_type)) << "Float should be a subtype of number";
        EXPECT_FALSE(number_type->isSubtypeOf(*float_type)) << "Number should not be a subtype of float";

    } catch (const std::exception& e) {
        FAIL() << "Exception caught: " << e.what();
    }
}

// トポロジー制約のテスト
TEST_F(TypeSystemTest, TopologyConstraintTest) {
    RefinedConstraintSystem system;

    auto connected_space = createConnectedSpace();
    system.addConstraint(
        std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
            std::move(connected_space)
        )
    );

    EXPECT_TRUE(system.verifyAll()) << "Topology constraint verification failed: " << system.getLastError();
}

// 依存型制約のテスト
TEST_F(TypeSystemTest, DependentConstraintTest) {
    RefinedConstraintSystem system;

    try {
        // 基本型の作成
        auto base_space = createConnectedSpace();
        if (!base_space) {
            FAIL() << "Failed to create base space";
        }
        
        // 依存型の作成
        auto dep_space = createConnectedSpace();
        if (!dep_space) {
            FAIL() << "Failed to create dependent space";
        }

        // 基本型の検証
        EXPECT_TRUE(base_space->verify()) << "Base space verification failed";
        EXPECT_TRUE(dep_space->verify()) << "Dependent space verification failed";

        auto predicate = [](const Type& t) {
            return t.verify();
        };

        auto topology_constraint = [](const Type& t) {
            if (auto topo = dynamic_cast<const TopologyType*>(&t)) {
                return topo->verify() && topo->verifyProperty(TopologyTraits::Property::CONNECTEDNESS);
            }
            return false;
        };

        auto quantum_constraint = [](const Type& t) {
            return t.verify();
        };

        auto dependent_type = std::make_unique<DependentType>(
            std::move(dep_space),
            predicate,
            topology_constraint,
            quantum_constraint
        );

        EXPECT_TRUE(dependent_type->verify()) << "Dependent type verification failed";

        system.addConstraint(
            std::make_unique<RefinedConstraintSystem::DependentConstraint>(
                std::move(base_space),
                std::move(dependent_type)
            )
        );

        // すべての制約を検証
        EXPECT_TRUE(system.verifyAll()) << "Dependent constraint verification failed: " << system.getLastError();

    } catch (const std::exception& e) {
        FAIL() << "Exception caught: " << e.what();
    }
}

// 複合制約のテスト
TEST_F(TypeSystemTest, CombinedConstraintTest) {
    RefinedConstraintSystem system;

    try {
        // サブタイプ制約
        {
            auto float_type = std::make_unique<BasicType>("float");
            auto number_type = std::make_unique<BasicType>("number");

            EXPECT_TRUE(float_type->verify()) << "Float type verification failed";
            EXPECT_TRUE(number_type->verify()) << "Number type verification failed";

            system.addConstraint(
                std::make_unique<RefinedConstraintSystem::SubtypeConstraint>(
                    std::move(float_type),
                    std::move(number_type)
                )
            );
        }

        // トポロジー制約
        {
            auto space = createConnectedSpace();
            EXPECT_TRUE(space->verify()) << "Space verification failed";

            system.addConstraint(
                std::make_unique<RefinedConstraintSystem::TopologicalConstraint>(
                    std::move(space)
                )
            );
        }

        // 依存型制約
        {
            auto base_space = createConnectedSpace();
            auto dep_space = createConnectedSpace();

            EXPECT_TRUE(base_space->verify()) << "Base space verification failed";
            EXPECT_TRUE(dep_space->verify()) << "Dependent space verification failed";

            auto predicate = [](const Type& t) {
                return t.verify();
            };

            auto topology_constraint = [](const Type& t) {
                if (auto topo = dynamic_cast<const TopologyType*>(&t)) {
                    return topo->verify() && topo->verifyProperty(TopologyTraits::Property::CONNECTEDNESS);
                }
                return false;
            };

            auto quantum_constraint = [](const Type& t) {
                return t.verify();
            };

            auto dependent_type = std::make_unique<DependentType>(
                std::move(dep_space),
                predicate,
                topology_constraint,
                quantum_constraint
            );

            EXPECT_TRUE(dependent_type->verify()) << "Dependent type verification failed";

            system.addConstraint(
                std::make_unique<RefinedConstraintSystem::DependentConstraint>(
                    std::move(base_space),
                    std::move(dependent_type)
                )
            );
        }

        // すべての制約を検証
        EXPECT_TRUE(system.verifyAll()) << "Combined constraint verification failed: " << system.getLastError();

    } catch (const std::exception& e) {
        FAIL() << "Exception caught: " << e.what();
    }
}

// ヘルパー関数
std::unique_ptr<TopologyType> createConnectedSpace() {
    auto base_type = std::make_unique<BasicType>("space");
    auto topology = std::make_unique<TopologyType>(std::move(base_type));
    topology->setProperty(TopologyTraits::Property::CONNECTEDNESS, true);
    topology->setProperty(TopologyTraits::Property::HAUSDORFFNESS, true);
    return topology;
}

std::unique_ptr<TopologyType> createHausdorffSpace() {
    auto base_type = std::make_unique<BasicType>("space");
    auto topology = std::make_unique<TopologyType>(std::move(base_type));
    topology->setProperty(TopologyTraits::Property::CONNECTEDNESS, false);
    topology->setProperty(TopologyTraits::Property::HAUSDORFFNESS, true);
    return topology;
}

std::unique_ptr<TopologyType> createCompactSpace() {
    auto base_type = std::make_unique<BasicType>("space");
    auto topology = std::make_unique<TopologyType>(std::move(base_type));
    topology->setProperty(TopologyTraits::Property::CONNECTEDNESS, true);
    topology->setProperty(TopologyTraits::Property::HAUSDORFFNESS, true);
    topology->setProperty(TopologyTraits::Property::COMPACTNESS, true);
    return topology;
}

std::unique_ptr<QuantumType> createEntangledState() {
    auto quantum = std::make_unique<QuantumType>();
    quantum->setProperty(QuantumTraits::Property::unitary, true);
    quantum->setProperty(QuantumTraits::Property::normalized, true);
    quantum->setProperty(QuantumTraits::Property::entangled, true);
    return quantum;
}

std::unique_ptr<QuantumType> createSuperposition() {
    auto quantum = std::make_unique<QuantumType>();
    quantum->setProperty(QuantumTraits::Property::unitary, true);
    quantum->setProperty(QuantumTraits::Property::normalized, true);
    quantum->setProperty(QuantumTraits::Property::entangled, false);
    return quantum;
}

std::unique_ptr<QuantumType> createMeasurement() {
    auto quantum = std::make_unique<QuantumType>();
    quantum->setProperty(QuantumTraits::Property::unitary, false);
    quantum->setProperty(QuantumTraits::Property::normalized, true);
    quantum->setProperty(QuantumTraits::Property::entangled, false);
    return quantum;
}

// 検証関数
bool verifyConnectedness(const TopologyType& space) {
    return space.verifyProperty(TopologyTraits::Property::CONNECTEDNESS);
}

bool verifySeparationAxioms(const TopologyType& space) {
    return space.verifyProperty(TopologyTraits::Property::HAUSDORFFNESS);
}

bool verifyCompactness(const TopologyType& space) {
    return space.verifyProperty(TopologyTraits::Property::COMPACTNESS);
}

bool verifyEntanglement(const QuantumType& state) {
    return state.verifyProperty(QuantumTraits::Property::entangled);
}

bool verifySuperposition(const QuantumType& state) {
    return state.verifyProperty(QuantumTraits::Property::normalized) && 
           !state.verifyProperty(QuantumTraits::Property::entangled);
}

bool verifyMeasurement(const QuantumType& state) {
    return !state.verifyProperty(QuantumTraits::Property::unitary) && 
           state.verifyProperty(QuantumTraits::Property::normalized);
}

} // namespace test
} // namespace types
} // namespace topos
