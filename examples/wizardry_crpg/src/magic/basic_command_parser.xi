// 基本的な魔法コマンドパーサー
space BasicCommandParser {
    // 基本コマンドの定義
    shape BasicCommands {
        properties {
            // 利用可能なコマンド
            valid_commands: Collection<Command> = {
                // 基本動作
                wave: "wave",    // 量子波動
                circle: "circle", // 魔法陣
                point: "point",  // 対象指定

                // 基本要素
                quantum: "quantum",     // 量子
                topology: "topology",   // 位相
                entangle: "entangle"    // もつれ
            }

            // 簡単な組み合わせルール
            valid_combinations: Collection<Combination> = {
                "wave quantum",   // 量子波動
                "circle topology", // 位相魔法陣
                "point entangle"  // もつれ指定
            }
        }
    }

    // コマンド解析
    shape CommandParser {
        properties {
            // パース結果
            type ParseResult {
                valid: Boolean,
                command: String,
                element: String?,
                error: String?
            }
        }

        // コマンドの解析
        mapping parse_command(input: String): ParseResult {
            path {
                // 入力の正規化
                normalize() {
                    trim_spaces ->
                    to_lowercase
                }

                // コマンドの検証
                validate() {
                    check_command ->
                    check_element ->
                    verify_combination
                }

                // 結果の生成
                create_result() {
                    set_validity ->
                    set_components ->
                    return_result
                }
            }
        }
    }

    // 基本的な効果生成
    shape BasicEffectGenerator {
        properties {
            // 基本効果の定義
            basic_effects: Collection<Effect> = {
                wave_quantum: {
                    name: "量子波動",
                    power: 5,
                    description: "対象に量子の波動をもたらす"
                },
                circle_topology: {
                    name: "位相魔法陣",
                    power: 4,
                    description: "空間に位相的な歪みを作る"
                },
                point_entangle: {
                    name: "もつれの指定",
                    power: 6,
                    description: "対象と量子もつれを形成する"
                }
            }
        }

        // 効果の生成
        mapping generate_effect(command: String, element: String): Effect {
            path {
                lookup_effect ->
                apply_modifiers ->
                return_effect
            }
        }
    }

    // 基本的な戦闘ログ
    shape BasicCombatLog {
        properties {
            // ログメッセージのテンプレート
            log_templates: Collection<Template> = {
                cast_start: "[詠唱開始] {effect_name}",
                cast_success: "[成功] {effect_name} が発動",
                cast_failure: "[失敗] {effect_name} の詠唱に失敗"
            }
        }

        // ログの生成
        mapping generate_log(effect: Effect, success: Boolean): String {
            path {
                select_template ->
                fill_parameters ->
                return_message
            }
        }
    }

    // インターフェース
    interface BasicInterface {
        // 基本操作
        operations {
            parse_command(input: String): ParseResult
            generate_effect(command: String, element: String): Effect
            generate_log(effect: Effect, success: Boolean): String
        }
    }
}
