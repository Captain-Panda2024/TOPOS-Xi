#ifndef TOPOS_COMPILER_CODEGEN_QUANTUM_IR_HPP
#define TOPOS_COMPILER_CODEGEN_QUANTUM_IR_HPP

#include "../quantum/syntax.hpp"
#include <memory>
#include <vector>
#include <string>

namespace topos {
namespace compiler {
namespace codegen {

// 量子IR命令の基底クラス
class QuantumIRInstruction {
public:
    virtual ~QuantumIRInstruction() = default;
    virtual std::string toString() const = 0;
};

// 量子状態の割り当て
class AllocateQubitInstruction : public QuantumIRInstruction {
public:
    explicit AllocateQubitInstruction(const std::string& name)
        : qubit_name_(name) {}

    std::string toString() const override {
        return "qalloc " + qubit_name_;
    }

private:
    std::string qubit_name_;
};

// 量子ゲートの適用
class ApplyGateInstruction : public QuantumIRInstruction {
public:
    ApplyGateInstruction(
        const std::string& gate_name,
        const std::vector<std::string>& qubits)
        : gate_name_(gate_name), target_qubits_(qubits) {}

    std::string toString() const override {
        std::string result = "apply " + gate_name_;
        for (const auto& qubit : target_qubits_) {
            result += " " + qubit;
        }
        return result;
    }

private:
    std::string gate_name_;
    std::vector<std::string> target_qubits_;
};

// 測定操作
class MeasureInstruction : public QuantumIRInstruction {
public:
    MeasureInstruction(
        const std::string& qubit,
        const std::string& result)
        : qubit_name_(qubit), result_name_(result) {}

    std::string toString() const override {
        return "measure " + qubit_name_ + " -> " + result_name_;
    }

private:
    std::string qubit_name_;
    std::string result_name_;
};

// 条件分岐
class BranchInstruction : public QuantumIRInstruction {
public:
    BranchInstruction(
        const std::string& condition,
        const std::string& true_label,
        const std::string& false_label)
        : condition_(condition),
          true_label_(true_label),
          false_label_(false_label) {}

    std::string toString() const override {
        return "branch " + condition_ + " ? " +
               true_label_ + " : " + false_label_;
    }

private:
    std::string condition_;
    std::string true_label_;
    std::string false_label_;
};

// 量子レジスタの解放
class DeallocateQubitInstruction : public QuantumIRInstruction {
public:
    explicit DeallocateQubitInstruction(const std::string& name)
        : qubit_name_(name) {}

    std::string toString() const override {
        return "qfree " + qubit_name_;
    }

private:
    std::string qubit_name_;
};

// 量子IR基本ブロック
class QuantumBasicBlock {
public:
    void addInstruction(std::unique_ptr<QuantumIRInstruction> inst) {
        instructions_.push_back(std::move(inst));
    }

    const std::vector<std::unique_ptr<QuantumIRInstruction>>&
    getInstructions() const {
        return instructions_;
    }

private:
    std::vector<std::unique_ptr<QuantumIRInstruction>> instructions_;
};

// 量子IR関数
class QuantumFunction {
public:
    explicit QuantumFunction(const std::string& name)
        : name_(name) {}

    void addBlock(std::unique_ptr<QuantumBasicBlock> block) {
        blocks_.push_back(std::move(block));
    }

    const std::vector<std::unique_ptr<QuantumBasicBlock>>&
    getBlocks() const {
        return blocks_;
    }

    const std::string& getName() const { return name_; }

private:
    std::string name_;
    std::vector<std::unique_ptr<QuantumBasicBlock>> blocks_;
};

// 量子IRモジュール
class QuantumModule {
public:
    void addFunction(std::unique_ptr<QuantumFunction> func) {
        functions_.push_back(std::move(func));
    }

    const std::vector<std::unique_ptr<QuantumFunction>>&
    getFunctions() const {
        return functions_;
    }

private:
    std::vector<std::unique_ptr<QuantumFunction>> functions_;
};

} // namespace codegen
} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_CODEGEN_QUANTUM_IR_HPP
