#ifndef TOPOS_TESTS_COMPILER_CODEGEN_QUANTUM_ALGORITHM_OPTIMIZER_TEST_HPP
#define TOPOS_TESTS_COMPILER_CODEGEN_QUANTUM_ALGORITHM_OPTIMIZER_TEST_HPP

#include "../../../src/compiler/codegen/quantum_algorithm_optimizer.hpp"
#include "../../../src/compiler/codegen/quantum_algorithm_utils.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <cmath>

namespace topos {
namespace tests {
namespace compiler {
namespace codegen {

class QuantumAlgorithmOptimizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        module_ = std::make_unique<topos::compiler::codegen::QuantumModule>();
    }

    // ヘルパー関数：量子関数の作成
    std::unique_ptr<topos::compiler::codegen::QuantumFunction> 
    createQuantumFunction(const std::string& name) {
        return std::make_unique<topos::compiler::codegen::QuantumFunction>(name);
    }

    // ヘルパー関数：基本ブロックの作成
    std::unique_ptr<topos::compiler::codegen::QuantumBasicBlock>
    createBasicBlock() {
        return std::make_unique<topos::compiler::codegen::QuantumBasicBlock>();
    }

    std::unique_ptr<topos::compiler::codegen::QuantumModule> module_;
};

// QFT最適化のテスト
TEST_F(QuantumAlgorithmOptimizerTest, QFTOptimizationTest) {
    using namespace topos::compiler::codegen;

    // QFT関数の作成
    auto func = createQuantumFunction("qft_test");
    auto block = createBasicBlock();

    // テスト用の量子ビット
    std::vector<std::string> qubits = {"q0", "q1", "q2"};

    // QFT回路の生成
    auto qft_instructions = QFTHelper::generateQFTCircuit(qubits);
    for (auto& inst : qft_instructions) {
        block->addInstruction(std::move(inst));
    }

    func->addBlock(std::move(block));
    module_->addFunction(std::move(func));

    // 最適化の実行
    QFTOptimizationPass optimizer;
    optimizer.optimize(*module_);

    // 最適化結果の検証
    const auto& optimized_func = module_->getFunctions()[0];
    const auto& optimized_block = optimized_func->getBlocks()[0];
    const auto& instructions = optimized_block->getInstructions();

    // 命令数の検証
    EXPECT_GT(instructions.size(), 0);

    // 回転ゲートの近似精度の検証
    for (const auto& inst : instructions) {
        if (auto* gate = dynamic_cast<const ApplyGateInstruction*>(inst.get())) {
            if (gate->getGateName() == "CRZ") {
                auto params = gate->getParameters();
                if (params.size() >= 3) {
                    double angle = std::stod(params[2]);
                    // π/32の倍数であることを確認
                    double normalized = std::fmod(angle, M_PI/32);
                    EXPECT_NEAR(normalized, 0.0, 1e-10);
                }
            }
        }
    }
}

// 位相推定最適化のテスト
TEST_F(QuantumAlgorithmOptimizerTest, QPEOptimizationTest) {
    using namespace topos::compiler::codegen;

    // QPE関数の作成
    auto func = createQuantumFunction("phase_estimation_test");
    auto block = createBasicBlock();

    // 制御ユニタリ操作の追加
    std::vector<std::string> controls = {"c0", "c1", "c1"};  // 意図的に重複
    std::string target = "t0";
    
    auto cu = ControlledUnitary("U", controls);
    block->addInstruction(
        std::make_unique<ApplyGateInstruction>(
            "CU",
            std::vector<std::string>{controls[0], controls[1], controls[2], target}
        )
    );

    func->addBlock(std::move(block));
    module_->addFunction(std::move(func));

    // 最適化の実行
    QPEOptimizationPass optimizer;
    optimizer.optimize(*module_);

    // 最適化結果の検証
    const auto& optimized_func = module_->getFunctions()[0];
    const auto& optimized_block = optimized_func->getBlocks()[0];
    const auto& instructions = optimized_block->getInstructions();

    // 冗長な制御ビットが削除されていることを確認
    for (const auto& inst : instructions) {
        if (auto* gate = dynamic_cast<const ApplyGateInstruction*>(inst.get())) {
            if (gate->getGateName() == "CU") {
                auto params = gate->getParameters();
                // 重複した制御ビットが削除されていることを確認
                std::set<std::string> unique_controls(
                    params.begin(), params.end() - 1);
                EXPECT_EQ(unique_controls.size(), 2);
            }
        }
    }
}

// Grover最適化のテスト
TEST_F(QuantumAlgorithmOptimizerTest, GroverOptimizationTest) {
    using namespace topos::compiler::codegen;

    // Grover関数の作成
    auto func = createQuantumFunction("grover_test");
    auto block = createBasicBlock();

    // テスト用の量子ビット
    std::vector<std::string> qubits = {"q0", "q1", "q2"};

    // Groverのディフュージョン演算子の追加
    block->addInstruction(
        std::make_unique<ApplyGateInstruction>(
            "GroverDiffusion",
            qubits
        )
    );

    func->addBlock(std::move(block));
    module_->addFunction(std::move(func));

    // 最適化の実行
    GroverOptimizationPass optimizer;
    optimizer.optimize(*module_);

    // 最適化結果の検証
    const auto& optimized_func = module_->getFunctions()[0];
    const auto& optimized_block = optimized_func->getBlocks()[0];
    const auto& instructions = optimized_block->getInstructions();

    // 最適化された命令列の構造を検証
    std::vector<std::string> expected_sequence = {"H", "Z", "CNOT", "H"};
    size_t seq_index = 0;

    for (const auto& inst : instructions) {
        if (auto* gate = dynamic_cast<const ApplyGateInstruction*>(inst.get())) {
            if (seq_index < expected_sequence.size()) {
                EXPECT_EQ(gate->getGateName(), expected_sequence[seq_index]);
                seq_index++;
            }
        }
    }
}

// 回転ゲート最適化のテスト
TEST_F(QuantumAlgorithmOptimizerTest, RotationGateOptimizationTest) {
    using namespace topos::compiler::codegen;

    // テスト関数の作成
    auto func = createQuantumFunction("rotation_test");
    auto block = createBasicBlock();

    // 回転ゲートの追加
    block->addInstruction(
        std::make_unique<ApplyGateInstruction>(
            "RZ",
            std::vector<std::string>{"q0", "1.57079632679"}  // π/2
        )
    );

    func->addBlock(std::move(block));
    module_->addFunction(std::move(func));

    // 最適化の実行
    QFTOptimizationPass optimizer;
    optimizer.optimize(*module_);

    // 最適化結果の検証
    const auto& optimized_func = module_->getFunctions()[0];
    const auto& optimized_block = optimized_func->getBlocks()[0];
    const auto& instructions = optimized_block->getInstructions();

    // T/Tdgゲートへの分解を検証
    bool found_t_gate = false;
    for (const auto& inst : instructions) {
        if (auto* gate = dynamic_cast<const ApplyGateInstruction*>(inst.get())) {
            if (gate->getGateName() == "T" || 
                gate->getGateName() == "Tdg") {
                found_t_gate = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found_t_gate);
}

} // namespace codegen
} // namespace compiler
} // namespace tests
} // namespace topos

#endif // TOPOS_TESTS_COMPILER_CODEGEN_QUANTUM_ALGORITHM_OPTIMIZER_TEST_HPP
