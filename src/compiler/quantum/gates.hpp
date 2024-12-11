#ifndef TOPOS_COMPILER_QUANTUM_GATES_HPP
#define TOPOS_COMPILER_QUANTUM_GATES_HPP

#include <complex>
#include <vector>
#include <memory>
#include <string>

namespace topos {
namespace quantum {

// 量子ゲートの基底クラス
class QuantumGate {
public:
    virtual ~QuantumGate() = default;
    virtual void apply(std::vector<std::complex<double>>& state) const = 0;
    virtual std::string getName() const = 0;
    virtual bool isUnitary() const = 0;
};

// パウリX（NOT）ゲート
class PauliXGate : public QuantumGate {
public:
    void apply(std::vector<std::complex<double>>& state) const override {
        for (size_t i = 0; i < state.size(); i += 2) {
            std::swap(state[i], state[i + 1]);
        }
    }

    std::string getName() const override { return "X"; }
    bool isUnitary() const override { return true; }
};

// パウリYゲート
class PauliYGate : public QuantumGate {
public:
    void apply(std::vector<std::complex<double>>& state) const override {
        std::complex<double> i(0, 1);
        for (size_t i = 0; i < state.size(); i += 2) {
            auto temp = state[i];
            state[i] = -i * state[i + 1];
            state[i + 1] = i * temp;
        }
    }

    std::string getName() const override { return "Y"; }
    bool isUnitary() const override { return true; }
};

// パウリZゲート
class PauliZGate : public QuantumGate {
public:
    void apply(std::vector<std::complex<double>>& state) const override {
        for (size_t i = 1; i < state.size(); i += 2) {
            state[i] = -state[i];
        }
    }

    std::string getName() const override { return "Z"; }
    bool isUnitary() const override { return true; }
};

// アダマールゲート
class HadamardGate : public QuantumGate {
public:
    void apply(std::vector<std::complex<double>>& state) const override {
        double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        for (size_t i = 0; i < state.size(); i += 2) {
            auto a = state[i];
            auto b = state[i + 1];
            state[i] = (a + b) * inv_sqrt2;
            state[i + 1] = (a - b) * inv_sqrt2;
        }
    }

    std::string getName() const override { return "H"; }
    bool isUnitary() const override { return true; }
};

// 位相ゲート
class PhaseGate : public QuantumGate {
public:
    explicit PhaseGate(double theta) : theta_(theta) {}

    void apply(std::vector<std::complex<double>>& state) const override {
        std::complex<double> phase(std::cos(theta_), std::sin(theta_));
        for (size_t i = 1; i < state.size(); i += 2) {
            state[i] *= phase;
        }
    }

    std::string getName() const override { return "P(" + std::to_string(theta_) + ")"; }
    bool isUnitary() const override { return true; }

private:
    double theta_;
};

// CNOT（制御NOTゲート）
class CNOTGate : public QuantumGate {
public:
    void apply(std::vector<std::complex<double>>& state) const override {
        for (size_t i = 0; i < state.size(); i += 4) {
            std::swap(state[i + 2], state[i + 3]);
        }
    }

    std::string getName() const override { return "CNOT"; }
    bool isUnitary() const override { return true; }
};

// 量子回路
class QuantumCircuit {
public:
    void addGate(std::unique_ptr<QuantumGate> gate) {
        gates_.push_back(std::move(gate));
    }

    void apply(std::vector<std::complex<double>>& state) const {
        for (const auto& gate : gates_) {
            gate->apply(state);
        }
    }

    bool isUnitary() const {
        for (const auto& gate : gates_) {
            if (!gate->isUnitary()) return false;
        }
        return true;
    }

private:
    std::vector<std::unique_ptr<QuantumGate>> gates_;
};

} // namespace quantum
} // namespace topos

#endif // TOPOS_COMPILER_QUANTUM_GATES_HPP
