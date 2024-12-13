#include "topology_verifier.hpp"
#include "../types/type_system.hpp"
#include <memory>
#include <vector>

namespace topos {
namespace topology {

bool TopologyVerifier::verify(const types::TopologicalType& type) {
    // 基本的なトポロジカル性質の検証
    if (!verifyBasicProperties(type)) {
        return false;
    }

    // 分離公理の検証
    if (!verifySeparationAxioms(type)) {
        return false;
    }

    // 複合型の検証
    return verifyCompositeType(type);
}

bool TopologyVerifier::verifyBasicProperties(const types::TopologicalType& type) {
    // 空集合と全体集合の存在を確認
    if (!type.hasEmptySet() || !type.hasWholeSet()) {
        return false;
    }

    // 有限個の開集合の共通部分が開集合であることを確認
    return type.isFiniteIntersectionClosed();
}

bool TopologyVerifier::verifySeparationAxioms(const types::TopologicalType& type) {
    // T0分離公理の検証
    if (!type.satisfiesT0()) {
        return false;
    }

    // T1分離公理の検証（必要に応じて）
    if (type.requiresT1() && !type.satisfiesT1()) {
        return false;
    }

    return true;
}

bool TopologyVerifier::verifyCompositeType(const types::TopologicalType& type) {
    // 複合型の構造を検証
    if (type.isComposite()) {
        const auto& components = type.getComponents();
        for (const auto& component : components) {
            if (!verify(*component)) {
                return false;
            }
        }
    }
    return true;
}

} // namespace topology
} // namespace topos
