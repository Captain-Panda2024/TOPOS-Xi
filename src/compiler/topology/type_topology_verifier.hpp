#ifndef TOPOS_COMPILER_TOPOLOGY_TYPE_TOPOLOGY_VERIFIER_HPP
#define TOPOS_COMPILER_TOPOLOGY_TYPE_TOPOLOGY_VERIFIER_HPP

#include "../types/type_system.hpp"
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

namespace topos {
namespace topology {

// トポロジー検証の例外クラス
class TopologyVerificationError : public std::runtime_error {
public:
    explicit TopologyVerificationError(const std::string& message)
        : std::runtime_error(message) {}
};

class TypeTopologyVerifier {
public:
    TypeTopologyVerifier() = default;
    ~TypeTopologyVerifier() = default;

    // 型のトポロジカルな性質を検証
    bool verify(const types::TopologyType& type);

    // 最後のエラーメッセージを取得
    const std::string& getLastError() const { return lastError_; }

private:
    // 基本的なトポロジカル性質の検証
    bool verifyBasicProperties(const types::TopologyType& type);

    // 分離公理の検証
    bool verifySeparationAxioms(const types::TopologyType& type);

    // 複合型の検証
    bool verifyCompositeType(const types::TopologyType& type);

    // エラーメッセージを設定
    void setError(const std::string& message) {
        lastError_ = message;
    }

    std::string lastError_;  // 最後のエラーメッセージ
};

} // namespace topology
} // namespace topos

#endif // TOPOS_COMPILER_TOPOLOGY_TYPE_TOPOLOGY_VERIFIER_HPP
