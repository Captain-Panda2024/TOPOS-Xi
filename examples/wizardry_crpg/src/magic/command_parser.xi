// 魔法詠唱コマンドパーサー
space CommandParser {
    // コマンドの基本構造
    shape SpellCommand {
        properties {
            // コマンドの種類
            command_type: Enum = {
                GESTURE,    // 手振りによる詠唱
                WORD,      // 言葉による詠唱
                FOCUS,     // 精神集中
                COMBINE,   // 組み合わせ詠唱
                CANCEL     // 詠唱中断
            }

            // コマンドの構文パターン
            syntax_patterns: Collection<Pattern> = {
                gesture: /^(wave|circle|point|cross)\s+(\w+)$/,
                word: /^(incantare|quantum|topologica|entanglo)\s*$/,
                focus: /^(concentrate|meditate|resonate)\s+(\w+)$/,
                combine: /^(q-wave|t-circle|e-point)\s*$/,
                cancel: /^(cancel|stop|abort)\s*$/
            }
        }

        // コマンド解析の結果
        type CommandResult {
            valid: Boolean
            type: CommandType
            primary: String
            secondary: String?
            power: Quantum<Power>
            stability: Topology<Stability>
        }
    }

    // 詠唱コマンドの解析器
    shape CommandAnalyzer {
        properties {
            // 現在の詠唱状態
            current_state: Quantum<CastingState> = {
                phase: Observable<Phase>,
                power: Observable<Power>,
                stability: Observable<Stability>
            }

            // コマンド履歴
            command_history: Stack<Command> = {
                max_size: 10,
                persistent: false
            }
        }

        // コマンドの解析
        mapping analyze_command(input: String): CommandResult {
            path {
                // 入力の正規化
                normalize_input() {
                    trim_whitespace ->
                    convert_to_lowercase ->
                    remove_extra_spaces
                }

                // パターンマッチング
                match_pattern() {
                    try_gesture_pattern ->
                    try_word_pattern ->
                    try_focus_pattern ->
                    try_combine_pattern ->
                    try_cancel_pattern
                }

                // 量子状態の評価
                evaluate_quantum_state() {
                    check_phase_compatibility ->
                    calculate_power_level ->
                    assess_stability
                }

                // 結果の生成
                generate_result() {
                    create_result_object ->
                    update_history ->
                    return_result
                }
            }
        }
    }

    // コマンドの実行制御
    shape CommandExecutor {
        properties {
            // 実行コンテキスト
            context: Topology<Context> = {
                environment: Observable<Environment>,
                caster: Observable<CasterState>,
                target: Observable<Target>
            }

            // 実行結果のフィードバック
            feedback: Collection<Feedback> = {
                success: String,
                partial: String,
                failure: String
            }
        }

        // コマンドの実行
        mapping execute_command(result: CommandResult): ExecutionResult {
            path {
                // 前提条件の確認
                verify_conditions() {
                    check_caster_state ->
                    verify_environment ->
                    validate_target
                }

                // 量子効果の計算
                calculate_effects() {
                    compute_power_level ->
                    determine_stability ->
                    predict_outcomes
                }

                // 効果の適用
                apply_effects() {
                    modify_quantum_state ->
                    update_topology ->
                    record_changes
                }

                // フィードバックの生成
                generate_feedback() {
                    assess_results ->
                    create_message ->
                    return_feedback
                }
            }
        }
    }

    // コマンドのヘルプシステム
    shape CommandHelper {
        properties {
            // コマンドガイド
            command_guide: Collection<Help> = {
                basic_commands: [
                    "wave [element] - 基本的な詠唱",
                    "circle [pattern] - 魔法陣の展開",
                    "point [target] - 対象の指定",
                    "cross [effect] - 効果の打ち消し"
                ],
                advanced_commands: [
                    "q-wave - 量子波動の詠唱",
                    "t-circle - 位相魔法陣の展開",
                    "e-point - もつれ効果の指定"
                ],
                focus_commands: [
                    "concentrate - 精神集中",
                    "meditate - 魔力調律",
                    "resonate - 共鳴増幅"
                ]
            }

            // エラーメッセージ
            error_messages: Collection<Error> = {
                syntax: "コマンドの構文が正しくありません",
                state: "現在の状態ではそのコマンドは使用できません",
                power: "魔力が不足しています",
                target: "対象が不適切です"
            }
        }

        // ヘルプの表示
        mapping show_help(topic: String): HelpText {
            path {
                find_topic ->
                format_help ->
                display_text
            }
        }
    }

    // インターフェース定義
    interface CommandInterface {
        // コマンド操作
        operations {
            parse_command(input: String): CommandResult
            execute_result(result: CommandResult): ExecutionResult
            show_help(topic: String): HelpText
        }

        // 状態の取得
        observations {
            get_current_state(): CastingState
            get_last_command(): Command
            get_available_commands(): CommandList
        }

        // エラー処理
        error_handling {
            handle_syntax_error(error: SyntaxError): ErrorMessage
            handle_state_error(error: StateError): ErrorMessage
            handle_execution_error(error: ExecutionError): ErrorMessage
        }
    }
}
