#ifndef TOPOS_COMPILER_CODEGEN_QUANTUM_BACKEND_HPP
#define TOPOS_COMPILER_CODEGEN_QUANTUM_BACKEND_HPP

#include "quantum_ir.hpp"
#include <string>
#include <memory>
#include <vector>
#include <sstream>

namespace topos {
namespace compiler {
namespace codegen {

// バックエンド生成器の基底クラス
class QuantumBackendGenerator {
public:
    virtual ~QuantumBackendGenerator() = default;
    virtual std::string generateCode(const QuantumModule& module) = 0;
    virtual std::string getName() const = 0;
};

// OpenQASMバックエンド
class OpenQASMBackend : public QuantumBackendGenerator {
public:
    std::string generateCode(const QuantumModule& module) override {
        std::stringstream ss;
        ss << "OPENQASM 3.0;\n";
        ss << "include \"stdgates.inc\";\n\n";

        for (const auto& func : module.getFunctions()) {
            generateFunction(ss, *func);
        }

        return ss.str();
    }

    std::string getName() const override {
        return "OpenQASM";
    }

private:
    void generateFunction(std::stringstream& ss, 
                         const QuantumFunction& func) {
        ss << "def " << func.getName() << "() {\n";
        
        for (const auto& block : func.getBlocks()) {
            generateBasicBlock(ss, *block);
        }
        
        ss << "}\n\n";
    }

    void generateBasicBlock(std::stringstream& ss,
                           const QuantumBasicBlock& block) {
        for (const auto& inst : block.getInstructions()) {
            generateInstruction(ss, *inst);
        }
    }

    void generateInstruction(std::stringstream& ss,
                           const QuantumIRInstruction& inst) {
        if (auto* alloc = dynamic_cast<const AllocateQubitInstruction*>(&inst)) {
            ss << "    qubit " << alloc->getQubitName() << ";\n";
        }
        else if (auto* gate = dynamic_cast<const ApplyGateInstruction*>(&inst)) {
            ss << "    " << gate->getGateName() << " ";
            auto params = gate->getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << params[i];
            }
            ss << ";\n";
        }
        else if (auto* measure = dynamic_cast<const MeasureInstruction*>(&inst)) {
            ss << "    bit " << measure->getResultName() 
               << " = measure " << measure->getQubitName() << ";\n";
        }
    }
};

// Qiskitバックエンド
class QiskitBackend : public QuantumBackendGenerator {
public:
    std::string generateCode(const QuantumModule& module) override {
        std::stringstream ss;
        ss << "from qiskit import QuantumCircuit, QuantumRegister, "
           << "ClassicalRegister\n";
        ss << "from qiskit.circuit import Gate\n\n";

        for (const auto& func : module.getFunctions()) {
            generateFunction(ss, *func);
        }

        return ss.str();
    }

    std::string getName() const override {
        return "Qiskit";
    }

private:
    void generateFunction(std::stringstream& ss,
                         const QuantumFunction& func) {
        ss << "def " << func.getName() << "():\n";
        ss << "    qr = QuantumRegister()\n";
        ss << "    cr = ClassicalRegister()\n";
        ss << "    circuit = QuantumCircuit(qr, cr)\n\n";
        
        for (const auto& block : func.getBlocks()) {
            generateBasicBlock(ss, *block);
        }
        
        ss << "    return circuit\n\n";
    }

    void generateBasicBlock(std::stringstream& ss,
                           const QuantumBasicBlock& block) {
        for (const auto& inst : block.getInstructions()) {
            generateInstruction(ss, *inst);
        }
    }

    void generateInstruction(std::stringstream& ss,
                           const QuantumIRInstruction& inst) {
        if (auto* alloc = dynamic_cast<const AllocateQubitInstruction*>(&inst)) {
            // Qiskitでは量子レジスタ作成時に暗黙的に割り当て
        }
        else if (auto* gate = dynamic_cast<const ApplyGateInstruction*>(&inst)) {
            ss << "    circuit." << gate->getGateName() << "(";
            auto params = gate->getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << params[i];
            }
            ss << ")\n";
        }
        else if (auto* measure = dynamic_cast<const MeasureInstruction*>(&inst)) {
            ss << "    circuit.measure(" << measure->getQubitName()
               << ", " << measure->getResultName() << ")\n";
        }
    }
};

// バックエンドマネージャー
class QuantumBackendManager {
public:
    void addBackend(std::unique_ptr<QuantumBackendGenerator> backend) {
        backends_[backend->getName()] = std::move(backend);
    }

    std::string generateCode(const std::string& backendName,
                           const QuantumModule& module) {
        auto it = backends_.find(backendName);
        if (it != backends_.end()) {
            return it->second->generateCode(module);
        }
        throw std::runtime_error("Backend not found: " + backendName);
    }

    // 標準的なバックエンドセットの構築
    static std::unique_ptr<QuantumBackendManager> createStandard() {
        auto manager = std::make_unique<QuantumBackendManager>();
        manager->addBackend(std::make_unique<OpenQASMBackend>());
        manager->addBackend(std::make_unique<QiskitBackend>());
        return manager;
    }

private:
    std::unordered_map<std::string,
                       std::unique_ptr<QuantumBackendGenerator>> backends_;
};

} // namespace codegen
} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_CODEGEN_QUANTUM_BACKEND_HPP
