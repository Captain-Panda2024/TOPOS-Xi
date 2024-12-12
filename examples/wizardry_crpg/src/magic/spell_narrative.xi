// 魔法詠唱のナラティブシステム
space SpellNarrativeSystem {
    // 詠唱の状態を表現するテキスト生成
    shape SpellNarration {
        properties {
            // 詠唱の進行状態に応じたテキストパターン
            casting_descriptions: Collection<NarrativePattern> = {
                initiation: [
                    "魔力が指先で震えはじめる...",
                    "空気中に魔法の共鳴が広がる...",
                    "詠唱者の周りで現実が歪みはじめる..."
                ],
                buildup: [
                    "量子の糸が織りなすパターンが浮かび上がる...",
                    "並行世界からの反響が感じられる...",
                    "魔力の結晶が空中に形成される..."
                ],
                completion: [
                    "現実が一瞬震え、魔法が具現化する",
                    "量子の波が収束し、効果が現れる",
                    "並行世界の可能性が一点に集中する"
                ]
            }

            // 魔法効果の記述パターン
            effect_descriptions: Collection<EffectPattern> = {
                success: [
                    "魔法が完璧に発動した！",
                    "詠唱は見事な成功を収めた",
                    "魔力が意図通りに具現化した"
                ],
                partial: [
                    "魔法は部分的な効果を示した",
                    "詠唱は不完全ながらも形になった",
                    "予期せぬ干渉が効果を歪めた"
                ],
                failure: [
                    "魔力が制御を逃れ、霧散した...",
                    "詠唱は不安定な状態で崩壊した...",
                    "量子の糸が絡まり、効果が失われた..."
                ]
            }
        }

        // テキスト生成のロジック
        mapping generate_narrative() {
            properties {
                quantum_state_aware: Boolean = true
                context_sensitive: Boolean = true
            }

            path {
                analyze_state ->
                select_pattern ->
                apply_modifiers ->
                generate_text
            }
        }
    }

    // ASCII アートによる視覚的表現
    shape SpellVisuals {
        properties {
            // 魔法陣のASCIIアート
            magic_circles: Collection<ASCIIPattern> = {
                basic: [
                    "   *   ",
                    " * * * ",
                    "* * * *",
                    " * * * ",
                    "   *   "
                ],
                advanced: [
                    "  .*.  ",
                    ".*∴*.*",
                    "*∴∴∴*",
                    ".*∴*.*",
                    "  .*.  "
                ]
            }

            // 効果のビジュアル表現
            effect_visuals: Collection<ASCIIPattern> = {
                wave: [
                    "～～～",
                    "≈≈≈≈",
                    "～～～"
                ],
                burst: [
                    " * * ",
                    "* * *",
                    " * * "
                ]
            }
        }

        // ビジュアル生成のロジック
        mapping generate_visuals() {
            properties {
                ascii_only: Boolean = true
                terminal_compatible: Boolean = true
            }

            path {
                select_pattern ->
                apply_effects ->
                format_output
            }
        }
    }

    // プレイヤーの入力解析
    shape SpellInput {
        properties {
            // 詠唱コマンドのパターン
            command_patterns: Collection<CommandPattern> = {
                gestures: ["wave", "circle", "point", "cross"],
                words: ["incantare", "quantum", "topologica", "entanglo"],
                focus: ["concentrate", "meditate", "resonate"]
            }

            // 入力の解析ルール
            parsing_rules: Collection<Rule> = {
                syntax: ["[gesture] [word]", "[focus] [word] [gesture]"],
                combinations: ["wave quantum", "circle topologica"],
                shortcuts: ["q-wave", "t-circle"]
            }
        }

        // 入力解析のロジック
        mapping parse_input() {
            properties {
                case_insensitive: Boolean = true
                flexible_syntax: Boolean = true
            }

            path {
                tokenize_input ->
                match_patterns ->
                validate_syntax ->
                interpret_command
            }
        }
    }

    // ゲームステートとの連携
    shape GameState {
        properties {
            // 詠唱の状態管理
            casting_state: Quantum<State> = {
                progress: Observable<Progress>,
                stability: Observable<Stability>,
                power: Observable<Power>
            }

            // 環境との相互作用
            environment: Topology<Environment> = {
                mana_level: Observable<Level>,
                interference: Observable<Noise>,
                resonance: Observable<Harmony>
            }
        }

        // 状態更新のロジック
        mapping update_state() {
            properties {
                atomic: Boolean = true
                consistent: Boolean = true
            }

            path {
                read_current_state ->
                apply_changes ->
                validate_state ->
                commit_update
            }
        }
    }

    // インターフェース定義
    interface SpellInterface {
        // テキストベースの操作
        operations {
            input_command(text: String): CommandResult
            show_status(): StatusText
            display_effects(): EffectText
        }

        // 状態の監視
        observations {
            get_narrative(): String
            get_visuals(): ASCIIArt
            get_feedback(): String
        }

        // ヘルプとガイダンス
        help {
            show_commands(): CommandList
            show_tutorial(): TutorialText
            show_hints(): HintText
        }
    }
}
