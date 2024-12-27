# TOPOS-Ξ 文法仕様
Version: 3.0
Status: Draft
Last Updated: 2024-12-27

## 1. 字句文法

### 1.1 トークン
```bnf
token ::= keyword | identifier | literal | operator | delimiter

keyword ::= 'space' | 'shape' | 'mapping' | 'path' | 'properties'
          | 'quantum' | 'topology' | 'transform' | 'verify'

identifier ::= letter (letter | digit | '_')*

literal ::= boolean_literal | number_literal | string_literal

operator ::= '->' | '=' | '<' | '>' | '+' | '-' | '*' | '/'

delimiter ::= '{' | '}' | '(' | ')' | '[' | ']' | ',' | ';'
```

### 1.2 空白文字と改行
```bnf
whitespace ::= ' ' | '\t' | '\n' | '\r'
```

## 2. 構文文法

### 2.1 プログラム構造
```bnf
program ::= space_definition*

space_definition ::=
    'space' identifier '{' 
        properties_block?
        (shape_definition | mapping_definition)*
    '}'

properties_block ::=
    'properties' '{' property_definition* '}'

property_definition ::=
    identifier ':' type_expression ('=' expression)?
```

### 2.2 型システム
```bnf
type_expression ::=
    basic_type
    | topology_type
    | quantum_type
    | composite_type

basic_type ::= 'Boolean' | 'Number' | 'String'

topology_type ::= 'Topology' '<' type_expression '>'

quantum_type ::= 'Quantum' '<' type_expression '>'

composite_type ::= identifier '<' type_expression (',' type_expression)* '>'
```

### 2.3 変換定義
```bnf
mapping_definition ::=
    'mapping' identifier '(' parameter_list? ')' '{' 
        path_block
    '}'

path_block ::=
    'path' '{' path_sequence '}'

path_sequence ::=
    identifier ('->' identifier)*
```

## 3. セマンティクス規則

### 3.1 型チェック
- すべての式は明示的な型を持つ必要がある
- トポロジー型は連続性を保証する必要がある
- 量子型は量子力学の法則に従う必要がある

### 3.2 パス検証
- すべてのパスは有効な変換の連鎖である必要がある
- パスの各ステップは型安全である必要がある
- トポロジカルな性質は保存される必要がある

## 4. 制約

### 4.1 型制約
- トポロジー型は連続写像のみを許可
- 量子型は可逆変換のみを許可
- 複合型は型パラメータの制約を満たす必要がある

### 4.2 操作制約
- パス内の各操作は明示的に定義される必要がある
- 量子操作はユニタリ性を保持する必要がある
- トポロジカル操作は連続性を保持する必要がある
