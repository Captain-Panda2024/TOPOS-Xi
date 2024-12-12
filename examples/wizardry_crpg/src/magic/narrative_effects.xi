// 魔法効果のナラティブ表現システム
space NarrativeEffects {
    // 効果の記述テンプレート
    shape EffectDescriptions {
        properties {
            // 量子効果の記述パターン
            quantum_descriptions: Collection<Description> = {
                wave: [
                    "空間が微かに震え、量子の波が広がる...",
                    "現実が一瞬揺らぎ、波動が対象を包み込む...",
                    "量子の振動が空気を伝わっていく..."
                ],
                collapse: [
                    "可能性の波が一点に収束する",
                    "量子状態が確定し、効果が現れる",
                    "波動関数が崩壊し、現実が定まる"
                ]
            }

            // 位相効果の記述
            topology_descriptions: Collection<Description> = {
                circle: [
                    "空間が捻じれ、位相の輪が形成される...",
                    "現実の構造が歪み、魔法陣が展開する...",
                    "位相的な変換が周囲を変容させる..."
                ],
                transform: [
                    "空間の位相が変化し、効果が現れる",
                    "トポロジカルな変換が完了する",
                    "構造が保存されたまま、変化が起こる"
                ]
            }

            // もつれ効果の記述
            entanglement_descriptions: Collection<Description> = {
                link: [
                    "量子のもつれが対象を結びつける...",
                    "現実の糸が絡み合い、結合が生まれる...",
                    "状態が共有され、影響が伝播する..."
                ],
                interact: [
                    "もつれた状態が相互に影響を及ぼす",
                    "量子相関が効果を伝える",
                    "絡み合った運命が顕在化する"
                ]
            }
        }
    }

    // 効果の状態表現
    shape StateDescriptions {
        properties {
            // 詠唱者の状態
            caster_states: Collection<State> = {
                focused: [
                    "精神が研ぎ澄まされている",
                    "魔力が体内で共鳴している",
                    "量子状態との同調が深まっている"
                ],
                unstable: [
                    "制御が不安定になっている",
                    "魔力の流れが乱れている",
                    "量子状態が揺らいでいる"
                ]
            }

            // 対象の状態
            target_states: Collection<State> = {
                affected: [
                    "量子の影響を受けている",
                    "位相変化の渦中にある",
                    "もつれの効果下にある"
                ],
                resistant: [
                    "効果への抵抗を示している",
                    "変化を打ち消そうとしている",
                    "影響を軽減している"
                ]
            }
        }
    }

    // 効果の結果表現
    shape ResultDescriptions {
        properties {
            // 成功時の描写
            success_descriptions: Collection<Description> = {
                perfect: [
                    "魔法が完璧に発動した！",
                    "意図した通りの効果が現れた！",
                    "詠唱が見事に成功した！"
                ],
                partial: [
                    "魔法は部分的に効果を示した",
                    "やや不安定ながらも効果が現れた",
                    "予期せぬ干渉があったが、魔法は成立した"
                ]
            }

            // 失敗時の描写
            failure_descriptions: Collection<Description> = {
                minor: [
                    "魔法は微かな痕跡を残して消えた...",
                    "効果は達成される前に散逸した...",
                    "詠唱は完了せず、効果は現れなかった..."
                ],
                critical: [
                    "魔法が制御不能に陥った！",
                    "量子状態が暴走した！",
                    "位相の歪みが暴発した！"
                ]
            }
        }
    }

    // ナラティブの生成制御
    shape NarrativeGenerator {
        properties {
            // 文脈情報
            context: Collection<Context> = {
                environment: String,
                tension: Integer,
                previous_effects: Array
            }

            // 生成ルール
            generation_rules: Collection<Rule> = {
                variation: "maintain_freshness",
                consistency: "preserve_context",
                drama: "match_tension"
            }
        }

        // ナラティブの生成
        mapping generate_narrative(effect: Effect): Narrative {
            path {
                // 文脈の分析
                analyze_context() {
                    evaluate_situation ->
                    check_history ->
                    determine_tone
                }

                // 描写の選択
                select_descriptions() {
                    choose_effect_description ->
                    select_state_description ->
                    pick_result_description
                }

                // テキストの構築
                construct_text() {
                    combine_descriptions ->
                    apply_formatting ->
                    ensure_flow
                }
            }
        }
    }

    // インターフェース定義
    interface NarrativeInterface {
        // ナラティブ操作
        operations {
            generate_effect_narrative(effect: Effect): String
            describe_state(state: State): String
            report_result(result: Result): String
        }

        // 文脈管理
        context {
            update_context(context: Context): Boolean
            get_current_context(): Context
            reset_context(): Boolean
        }

        // 設定
        configurations {
            set_narrative_style(style: Style): Boolean
            set_description_length(length: Length): Boolean
            toggle_dramatic_elements(enabled: Boolean): Boolean
        }
    }
}
