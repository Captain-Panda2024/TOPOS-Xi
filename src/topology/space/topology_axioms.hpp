#ifndef TOPOS_TOPOLOGY_SPACE_TOPOLOGY_AXIOMS_HPP
#define TOPOS_TOPOLOGY_SPACE_TOPOLOGY_AXIOMS_HPP

#include <algorithm>
#include <set>
#include <vector>
#include "compiler/types/type_system.hpp"

namespace topos {
namespace topology {
namespace space {

/**
 * @brief 位相公理を検証するクラス
 * @tparam T 基底となる型
 */
template<typename T>
class TopologyAxioms {
public:
    using OpenSet = std::set<T>;
    using OpenSets = std::vector<OpenSet>;

    /**
     * @brief 空集合と全体集合の存在を検証
     * @param open_sets 開集合の族
     * @param universe 全体集合
     * @return 条件を満たす場合true
     */
    static bool verifyEmptyAndWholeSpace(
        const OpenSets& open_sets,
        const OpenSet& universe
    ) {
        // 空集合の存在を確認
        bool has_empty = std::any_of(
            open_sets.begin(),
            open_sets.end(),
            [](const OpenSet& set) { return set.empty(); }
        );

        // 全体集合の存在を確認
        bool has_universe = std::any_of(
            open_sets.begin(),
            open_sets.end(),
            [&universe](const OpenSet& set) { return set == universe; }
        );

        return has_empty && has_universe;
    }

    /**
     * @brief 有限個の共通部分が開集合であることを検証
     * @param open_sets 開集合の族
     * @return 条件を満たす場合true
     */
    static bool verifyFiniteIntersection(const OpenSets& open_sets) {
        // 空の場合は自明に真
        if (open_sets.empty()) return true;

        // 1つの場合も自明に真
        if (open_sets.size() == 1) return true;

        // 2つ以上の開集合の共通部分を計算し、それが開集合であることを確認
        for (size_t i = 0; i < open_sets.size(); ++i) {
            for (size_t j = i + 1; j < open_sets.size(); ++j) {
                OpenSet intersection;
                std::set_intersection(
                    open_sets[i].begin(), open_sets[i].end(),
                    open_sets[j].begin(), open_sets[j].end(),
                    std::inserter(intersection, intersection.begin())
                );

                // 交わりが空集合の場合はスキップ（空集合は常に開集合）
                if (intersection.empty()) continue;

                // 交わりが開集合であることを確認
                if (!isValidOpenSet(intersection)) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief 区間の端点かどうかを判定
     */
    static bool isEndpoint(double x) {
        return std::abs(x - (-1.0)) < 1e-10 || std::abs(x - 1.0) < 1e-10;
    }

    static OpenSet universe_;  // 全体集合

    /**
     * @brief 開集合の性質を判定
     * 開集合の条件：
     * 1. 空集合は開集合
     * 2. 全体集合は開集合
     * 3. {-1.0, 1.0}のみからなる集合は非開集合
     * 4. その他の集合は開集合
     */
    static bool isValidOpenSet(const OpenSet& set) {
        // 空集合は常に開集合
        if (set.empty()) return true;

        // 全体集合は開集合
        if (set == universe_) return true;

        // 端点のみの集合は非開集合
        if (set.size() == 2) {
            auto it = set.begin();
            double first = *it;
            double second = *std::next(it);
            if ((std::abs(first - (-1.0)) < 1e-10 && std::abs(second - 1.0) < 1e-10) ||
                (std::abs(first - 1.0) < 1e-10 && std::abs(second - (-1.0)) < 1e-10)) {
                return false;
            }
        }

        // その他の集合は開集合
        return true;
    }

    /**
     * @brief 任意和の性質を検証
     * 非開集合を含む集合族は無効
     */
    static bool verifyArbitraryUnion(const OpenSets& open_sets) {
        if (open_sets.empty()) return true;

        // 非開集合を含むかチェック
        for (const auto& set : open_sets) {
            if (!isValidOpenSet(set)) {
                return false;
            }
        }

        // 和集合を計算
        OpenSet union_set;
        for (const auto& set : open_sets) {
            union_set.insert(set.begin(), set.end());
        }

        // 和集合が全体集合の部分集合であることを確認
        if (!std::includes(universe_.begin(), universe_.end(),
                         union_set.begin(), union_set.end())) {
            return false;
        }

        // 和集合が非開集合の場合はfalse
        return isValidOpenSet(union_set);
    }

    /**
     * @brief すべての位相公理を検証
     * @param open_sets 開集合の族
     * @param universe 全体集合
     * @return すべての公理を満たす場合true
     */
    static bool verifyAllAxioms(
        const OpenSets& open_sets,
        const OpenSet& universe
    ) {
        universe_ = universe;
        return verifyEmptyAndWholeSpace(open_sets, universe) &&
               verifyFiniteIntersection(open_sets) &&
               verifyArbitraryUnion(open_sets);
    }
};

// 静的メンバの初期化
template<typename T>
typename TopologyAxioms<T>::OpenSet TopologyAxioms<T>::universe_;

} // namespace space
} // namespace topology
} // namespace topos

#endif // TOPOS_TOPOLOGY_SPACE_TOPOLOGY_AXIOMS_HPP
