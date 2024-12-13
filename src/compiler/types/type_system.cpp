#include "type_system.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace topos {
namespace types {

TopologicalType::TopologicalType(std::string name)
    : name_(std::move(name)), isComposite_(false) {}

bool TopologicalType::hasEmptySet() const {
    return std::find_if(openSets_.begin(), openSets_.end(),
        [](const auto& set) { return set.empty(); }) != openSets_.end();
}

bool TopologicalType::hasWholeSet() const {
    return std::find_if(openSets_.begin(), openSets_.end(),
        [](const auto& set) { return set.isWhole(); }) != openSets_.end();
}

bool TopologicalType::isFiniteIntersectionClosed() const {
    // 有限個の開集合の共通部分が開集合であることを確認
    for (const auto& set1 : openSets_) {
        for (const auto& set2 : openSets_) {
            auto intersection = set1.intersect(set2);
            if (!isOpen(intersection)) {
                return false;
            }
        }
    }
    return true;
}

bool TopologicalType::satisfiesT0() const {
    // T0分離公理の検証実装
    // 任意の異なる2点に対して、一方を含み他方を含まない開集合が存在する
    return true; // 実際の実装では適切な検証を行う
}

bool TopologicalType::satisfiesT1() const {
    // T1分離公理の検証実装
    // 任意の異なる2点に対して、それぞれを含み他方を含まない開集合が存在する
    return true; // 実際の実装では適切な検証を行う
}

bool TopologicalType::requiresT1() const {
    return properties_.count("T1") > 0;
}

bool TopologicalType::isComposite() const {
    return isComposite_;
}

const std::vector<std::unique_ptr<TopologicalType>>& TopologicalType::getComponents() const {
    return components_;
}

void TopologicalType::addComponent(std::unique_ptr<TopologicalType> component) {
    if (!isComposite_) {
        isComposite_ = true;
    }
    components_.push_back(std::move(component));
}

bool TopologicalType::isOpen(const Set& set) const {
    return std::find(openSets_.begin(), openSets_.end(), set) != openSets_.end();
}

} // namespace types
} // namespace topos
