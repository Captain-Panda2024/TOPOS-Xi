#ifndef TOPOS_TOPOLOGY_SPACE_TOPOLOGICAL_SPACE_HPP
#define TOPOS_TOPOLOGY_SPACE_TOPOLOGICAL_SPACE_HPP

#include <memory>
#include <set>
#include <vector>
#include <algorithm>
#include "compiler/types/type_system.hpp"
#include "topology_axioms.hpp"

namespace topos {
namespace topology {
namespace space {

/**
 * @brief 位相空間を表現するクラス
 * @tparam T 基底となる型
 */
template<typename T>
class TopologicalSpace {
public:
    using OpenSet = std::set<T>;
    using OpenSets = std::vector<OpenSet>;

    /**
     * @brief コンストラクタ
     * @param base_type 基底となる型
     * @param universe 全体集合
     */
    TopologicalSpace(
        std::unique_ptr<types::Type> base_type,
        const OpenSet& universe
    ) : base_type_(std::move(base_type))
      , universe_(universe) {
        TopologyAxioms<T>::universe_ = universe_;  // 全体集合を設定
        // 空集合と全体集合を初期状態で追加
        open_sets_.push_back(OpenSet());  // 空集合
        open_sets_.push_back(universe);   // 全体集合
    }

    /**
     * @brief 区間の端点かどうかを判定
     */
    bool isEndpoint(double x) const {
        return std::abs(x - (-1.0)) < 1e-10 || std::abs(x - 1.0) < 1e-10;
    }

    /**
     * @brief 開集合の性質を判定
     * 開集合の条件：
     * 1. 空集合は開集合
     * 2. 全体集合は開集合
     * 3. {-1.0, 1.0}のみからなる集合は非開集合
     * 4. その他の集合は開集合
     */
    bool isValidOpenSet(const OpenSet& set) const {
        if (set.empty()) return true;  // 空集合は開集合
        if (set == universe_) return true;  // 全体集合は開集合

        // {-1.0, 1.0}のみからなる集合は非開集合
        if (set.size() == 2) {
            double first = *set.begin();
            double second = *std::next(set.begin());
            if ((std::abs(first - (-1.0)) < 1e-10 && std::abs(second - 1.0) < 1e-10) ||
                (std::abs(first - 1.0) < 1e-10 && std::abs(second - (-1.0)) < 1e-10)) {
                return false;
            }
        }

        return true;  // その他の集合は開集合
    }

    /**
     * @brief 開集合を追加
     * @param open_set 追加する開集合
     * @return 追加が成功した場合true
     */
    bool addOpenSet(const OpenSet& open_set) {
        // 既に同じ集合が含まれている場合はスキップ
        for (const auto& existing : open_sets_) {
            if (open_set == existing) {
                return true;
            }
        }

        // 空集合と全体集合は自明に開集合
        if (open_set.empty() || open_set == universe_) {
            open_sets_.push_back(open_set);
            return true;
        }

        // 追加する集合が全体集合の部分集合であることを確認
        if (!std::includes(
            universe_.begin(), universe_.end(),
            open_set.begin(), open_set.end()
        )) {
            return false;
        }

        // 開集合の性質を確認
        TopologyAxioms<T>::universe_ = universe_;  // 全体集合を設定
        if (!TopologyAxioms<T>::isValidOpenSet(open_set)) {
            return false;
        }

        // すべての条件を満たしていれば追加
        open_sets_.push_back(open_set);
        return true;
    }

    /**
     * @brief 点が開集合に含まれるかを検証
     * @param point 検証する点
     * @param open_set 開集合
     * @return 含まれる場合true
     */
    bool isInOpenSet(const T& point, const OpenSet& open_set) const {
        return open_set.find(point) != open_set.end();
    }

    /**
     * @brief 開集合の族が位相の公理を満たすか検証
     * @return 満たす場合true
     */
    bool verifyTopologyAxioms() const {
        TopologyAxioms<T>::universe_ = universe_;  // 全体集合を設定
        return TopologyAxioms<T>::verifyAllAxioms(open_sets_, universe_);
    }

    /**
     * @brief 全体集合を取得
     */
    const OpenSet& getUniverse() const {
        return universe_;
    }

    /**
     * @brief すべての開集合を取得
     */
    const OpenSets& getOpenSets() const {
        return open_sets_;
    }

private:
    std::unique_ptr<types::Type> base_type_;
    OpenSet universe_;
    OpenSets open_sets_;
};

} // namespace space
} // namespace topology
} // namespace topos

#endif // TOPOS_TOPOLOGY_SPACE_TOPOLOGICAL_SPACE_HPP
