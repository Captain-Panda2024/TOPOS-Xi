#include <gtest/gtest.h>
#include "../../../src/compiler/math/topology_verifier.hpp"
#include "../../../src/compiler/math/quantum_verifier.hpp"
#include <complex>
#include <vector>

namespace topos {
namespace math {
namespace test {

// トポロジカル検証のテスト
class TopologyVerifierTest : public ::testing::Test {
protected:
    void SetUp() override {
        verifier_ = std::make_unique<TopologyVerifier<double>>();
    }

    std::unique_ptr<TopologyVerifier<double>> verifier_;

    // 単位円の点を生成
    std::vector<Point<double>> createUnitCirclePoints(size_t n) {
        std::vector<Point<double>> points;
        for (size_t i = 0; i < n; ++i) {
            double angle = 2.0 * M_PI * i / n;
            points.emplace_back(std::vector<double>{
                std::cos(angle),
                std::sin(angle)
            });
        }
        return points;
    }

    // 単位円の開被覆を生成
    std::vector<OpenSet<double>> createUnitCircleCover(size_t n) {
        std::vector<OpenSet<double>> sets;
        auto points = createUnitCirclePoints(n);
        for (const auto& p : points) {
            sets.emplace_back(p, 0.5); // 半径0.5の開球
        }
        return sets;
    }
};

TEST_F(TopologyVerifierTest, PathConnectednessTest) {
    auto points = createUnitCirclePoints(8);
    auto open_sets = createUnitCircleCover(8);
    
    EXPECT_TRUE(verifier_->verifyPathConnectedness(points, open_sets));
}

TEST_F(TopologyVerifierTest, SeparationAxiomsTest) {
    auto points = createUnitCirclePoints(4);
    auto open_sets = createUnitCircleCover(4);
    
    EXPECT_TRUE(verifier_->verifySeparationAxioms(points, open_sets));
}

TEST_F(TopologyVerifierTest, CompactnessTest) {
    auto points = createUnitCirclePoints(12);
    auto open_sets = createUnitCircleCover(12);
    
    EXPECT_TRUE(verifier_->verifyCompactness(points, open_sets));
}

// 量子検証のテスト
class QuantumVerifierTest : public ::testing::Test {
protected:
    void SetUp() override {
        verifier_ = std::make_unique<QuantumVerifier<std::complex<double>>>();
    }

    std::unique_ptr<QuantumVerifier<std::complex<double>>> verifier_;

    // Pauliゲートを生成
    QuantumMatrix<std::complex<double>> createPauliX() {
        using Complex = std::complex<double>;
        return QuantumMatrix<Complex>({
            {Complex(0, 0), Complex(1, 0)},
            {Complex(1, 0), Complex(0, 0)}
        });
    }

    // Hadamardゲートを生成
    QuantumMatrix<std::complex<double>> createHadamard() {
        using Complex = std::complex<double>;
        double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        return QuantumMatrix<Complex>({
            {Complex(inv_sqrt2, 0), Complex(inv_sqrt2, 0)},
            {Complex(inv_sqrt2, 0), Complex(-inv_sqrt2, 0)}
        });
    }

    // Bell状態を生成
    QuantumMatrix<std::complex<double>> createBellState() {
        using Complex = std::complex<double>;
        double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        return QuantumMatrix<Complex>({
            {Complex(inv_sqrt2, 0)},
            {Complex(0, 0)},
            {Complex(0, 0)},
            {Complex(inv_sqrt2, 0)}
        });
    }
};

TEST_F(QuantumVerifierTest, UnitarityTest) {
    auto pauli_x = createPauliX();
    auto hadamard = createHadamard();
    
    EXPECT_TRUE(verifier_->verifyUnitarity(pauli_x));
    EXPECT_TRUE(verifier_->verifyUnitarity(hadamard));
}

TEST_F(QuantumVerifierTest, NormalizationTest) {
    auto bell_state = createBellState();
    
    EXPECT_TRUE(verifier_->verifyNormalization(bell_state));
}

TEST_F(QuantumVerifierTest, EntanglementTest) {
    auto bell_state = createBellState();
    
    EXPECT_TRUE(verifier_->verifyEntanglement(bell_state));
}

} // namespace test
} // namespace math
} // namespace topos
