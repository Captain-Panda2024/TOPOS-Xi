#ifndef TOPOS_COMPILER_TYPES_BASIC_TYPE_HPP
#define TOPOS_COMPILER_TYPES_BASIC_TYPE_HPP

#include "type.hpp"
#include <unordered_set>

namespace topos {
namespace types {

// 基本型（int, float, bool など）
class BasicType : public Type {
public:
    explicit BasicType(const std::string& name) : Type(name) {
        // 型階層の定義
        if (name == "float") {
            superTypes_.insert("number");
        } else if (name == "int") {
            superTypes_.insert("number");
        }
    }

    bool isSubtypeOf(const Type& other) const override {
        if (auto basic = dynamic_cast<const BasicType*>(&other)) {
            // 同一の型の場合
            if (this->toString() == basic->toString()) {
                return true;
            }

            // スーパータイプをチェック
            return superTypes_.find(basic->toString()) != superTypes_.end();
        }
        return false;
    }

    std::string toString() const override { return name_; }
    bool verify() const override { return true; }

private:
    std::unordered_set<std::string> superTypes_;  // スーパータイプのリスト
};

} // namespace types
} // namespace topos

#endif // TOPOS_COMPILER_TYPES_BASIC_TYPE_HPP
