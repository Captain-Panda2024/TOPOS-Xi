#ifndef TOPOS_COMPILER_CODEGEN_QUANTUM_ALGORITHM_UTILS_HPP
#define TOPOS_COMPILER_CODEGEN_QUANTUM_ALGORITHM_UTILS_HPP

#include "../types/quantum_traits.hpp"
#include <complex>
#include <vector>
#include <cmath>

namespace topos {
namespace compiler {
namespace codegen {

// 量子位相の表現
class QuantumPhase {
public:
    explicit QuantumPhase(double theta) : theta_(theta) {}

    // 位相の近似
    QuantumPhase approximate(double epsilon) const {
        // 位相を2πの倍数の範囲に正規化
        double normalized = std::fmod(theta_, 2 * M_PI);
        if (normalized < 0) normalized += 2 * M_PI;

        // εを考慮した近似
        double approx = std::round(normalized / epsilon) * epsilon;
        return QuantumPhase(approx);
    }

    double getValue() const { return theta_; }

private:
    double theta_;
};

// 量子回転ゲートの表現
class QuantumRotation {
public:
    QuantumRotation(const std::string& axis, double angle)
        : axis_(axis), angle_(angle) {}

    // 回転の近似
    QuantumRotation approximate(double epsilon) const {
        QuantumPhase phase(angle_);
        return QuantumRotation(axis_, phase.approximate(epsilon).getValue());
    }

    // T/Tダガーゲートへの分解
    std::vector<std::string> decomposeToT() const {
        std::vector<std::string> gates;
        double angle = std::fmod(angle_, 2 * M_PI);
        if (angle < 0) angle += 2 * M_PI;

        // π/4の倍数での近似
        int t_count = std::round(angle / (M_PI / 4));
        for (int i = 0; i < std::abs(t_count); ++i) {
            gates.push_back(t_count > 0 ? "T" : "Tdg");
        }
        return gates;
    }

    const std::string& getAxis() const { return axis_; }
    double getAngle() const { return angle_; }

private:
    std::string axis_;
    double angle_;
};

// 制御ユニタリ演算の表現
class ControlledUnitary {
public:
    ControlledUnitary(const std::string& unitary, 
                     const std::vector<std::string>& controls)
        : unitary_(unitary), controls_(controls) {}

    // 制御の最適化（制御ビット数の削減）
    ControlledUnitary optimize() const {
        // 冗長な制御ビットの削除
        std::vector<std::string> optimized_controls;
        for (const auto& control : controls_) {
            if (!isRedundant(control, optimized_controls)) {
                optimized_controls.push_back(control);
            }
        }
        return ControlledUnitary(unitary_, optimized_controls);
    }

    const std::string& getUnitary() const { return unitary_; }
    const std::vector<std::string>& getControls() const { return controls_; }

private:
    bool isRedundant(const std::string& control,
                     const std::vector<std::string>& controls) const {
        // 制御ビットの冗長性チェック
        return std::find(controls.begin(), controls.end(), control) 
               != controls.end();
    }

    std::string unitary_;
    std::vector<std::string> controls_;
};

// Groverのディフュージョン演算子
class GroverDiffusion {
public:
    explicit GroverDiffusion(const std::vector<std::string>& qubits)
        : qubits_(qubits) {}

    // ディフュージョン演算子の最適化
    std::vector<std::unique_ptr<QuantumIRInstruction>> 
    generateOptimizedInstructions() const {
        std::vector<std::unique_ptr<QuantumIRInstruction>> instructions;

        // 全体のアダマールゲート
        for (const auto& qubit : qubits_) {
            instructions.push_back(
                std::make_unique<ApplyGateInstruction>("H", 
                    std::vector<std::string>{qubit}));
        }

        // 位相反転
        instructions.push_back(
            std::make_unique<ApplyGateInstruction>("Z", 
                std::vector<std::string>{qubits_[0]}));

        // 制御NOTの連鎖
        for (size_t i = 1; i < qubits_.size(); ++i) {
            instructions.push_back(
                std::make_unique<ApplyGateInstruction>("CNOT",
                    std::vector<std::string>{qubits_[i-1], qubits_[i]}));
        }

        // 最後のアダマールゲート
        for (const auto& qubit : qubits_) {
            instructions.push_back(
                std::make_unique<ApplyGateInstruction>("H",
                    std::vector<std::string>{qubit}));
        }

        return instructions;
    }

private:
    std::vector<std::string> qubits_;
};

// 量子フーリエ変換の補助関数
class QFTHelper {
public:
    static std::vector<std::unique_ptr<QuantumIRInstruction>>
    generateQFTCircuit(const std::vector<std::string>& qubits,
                      double epsilon = M_PI / 64) {
        std::vector<std::unique_ptr<QuantumIRInstruction>> instructions;

        for (size_t i = 0; i < qubits.size(); ++i) {
            // アダマールゲート
            instructions.push_back(
                std::make_unique<ApplyGateInstruction>("H",
                    std::vector<std::string>{qubits[i]}));

            // 制御位相回転
            for (size_t j = i + 1; j < qubits.size(); ++j) {
                double phase = M_PI / std::pow(2, j - i);
                QuantumRotation rotation("Z", phase);
                auto approx = rotation.approximate(epsilon);

                instructions.push_back(
                    std::make_unique<ApplyGateInstruction>(
                        "CRZ",
                        std::vector<std::string>{
                            qubits[j],
                            qubits[i],
                            std::to_string(approx.getAngle())
                        }));
            }
        }

        return instructions;
    }
};

} // namespace codegen
} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_CODEGEN_QUANTUM_ALGORITHM_UTILS_HPP
