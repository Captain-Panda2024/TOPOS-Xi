import re
import os
import glob
import math
import random
from enum import Enum, auto
from dataclasses import dataclass
from typing import List, Optional, Any, Dict

# --- Lexer ---

class TokenType(Enum):
    KEYWORD = auto()
    IDENTIFIER = auto()
    NUMBER = auto()
    STRING = auto()
    OPERATOR = auto()
    DELIMITER = auto()
    EOF = auto()

@dataclass
class Token:
    type: TokenType
    value: str
    line: int
    column: int

class Lexer:
    KEYWORDS = {
        'space', 'shape', 'mapping', 'path', 'properties',
        'quantum', 'topology', 'transform', 'verify',
        'Boolean', 'Number', 'String', 'match'
    }

    TOKEN_SPEC = [
        ('NUMBER',   r'\d+(\.\d*)?'),
        ('COMMENT',  r'//.*'),
        ('IDENTIFIER', r'[A-Za-z_][A-Za-z0-9_]*'),
        ('OPERATOR', r'->|[=<>+\-*/]') ,
        ('DELIMITER', r'[\{\}\(\)\[\],;:]'),
        ('STRING',   r'"[^"]*"'),
        ('SKIP',     r'[ \t]+'),
        ('NEWLINE',  r'\n'),
        ('MISMATCH', r'.'),
    ]

    def __init__(self, source_code: str):
        self.source_code = source_code
        self.tokens = []

    def tokenize(self) -> List[Token]:
        line_num = 1
        line_start = 0
        re_token = re.compile('|'.join(f'(?P<{name}>{pattern})' for name, pattern in self.TOKEN_SPEC))
        
        for mo in re_token.finditer(self.source_code):
            kind = mo.lastgroup
            value = mo.group()
            column = mo.start() - line_start + 1
            
            if kind == 'NUMBER':
                self.tokens.append(Token(TokenType.NUMBER, value, line_num, column))
            elif kind == 'IDENTIFIER':
                type = TokenType.KEYWORD if value in self.KEYWORDS else TokenType.IDENTIFIER
                self.tokens.append(Token(type, value, line_num, column))
            elif kind == 'OPERATOR':
                self.tokens.append(Token(TokenType.OPERATOR, value, line_num, column))
            elif kind == 'DELIMITER':
                self.tokens.append(Token(TokenType.DELIMITER, value, line_num, column))
            elif kind == 'STRING':
                self.tokens.append(Token(TokenType.STRING, value[1:-1], line_num, column))
            elif kind == 'NEWLINE':
                line_start = mo.end()
                line_num += 1
            elif kind == 'SKIP' or kind == 'COMMENT':
                continue
            elif kind == 'MISMATCH':
                raise SyntaxError(f'Unexpected character {value!r} at line {line_num}, column {column}')
        
        self.tokens.append(Token(TokenType.EOF, '', line_num, 0))
        return self.tokens

# --- AST Nodes ---

@dataclass
class ASTNode:
    pass

@dataclass
class TypeExpr(ASTNode):
    name: str
    params: List['TypeExpr'] = None
    
    def __repr__(self):
        if self.name == '*':
            return f"{self.params[0]} * {self.params[1]}"
        if self.params:
            return f"{self.name}<{', '.join(map(str, self.params))}>"
        return self.name

@dataclass
class PropertyDef(ASTNode):
    name: str
    type: TypeExpr
    default: Optional[Any] = None

@dataclass
class Parameter(ASTNode):
    name: str
    type: TypeExpr

@dataclass
class MappingCall(ASTNode):
    name: str
    args: List[Any] # str or MappingCall

@dataclass
class LambdaMapping(ASTNode):
    params: List[Parameter]
    path: List[Any]

@dataclass
class MatchCase(ASTNode):
    value: str
    path: List[Any]

@dataclass
class MatchExpr(ASTNode):
    target: str
    cases: List[MatchCase]

@dataclass
class MappingDef(ASTNode):
    name: str
    params: List[Parameter]
    properties: List[PropertyDef]
    path: List[Any]  # List[str | MappingCall | LambdaMapping | MatchExpr]
    source: Optional[TypeExpr] = None
    target: Optional[TypeExpr] = None

@dataclass
class SpaceDef(ASTNode):
    name: str
    properties: List[PropertyDef]
    members: List[ASTNode]

# --- StdLib Loader ---

class StdLibLoader:
    def __init__(self, stdlib_path: str):
        self.stdlib_path = stdlib_path
    
    def load_definitions(self) -> Dict[str, Any]:
        definitions = {}
        htf_files = glob.glob(os.path.join(self.stdlib_path, "*.htf"))
        print(f"[StdLib] Loading from {self.stdlib_path}...")
        for file_path in htf_files:
            filename = os.path.basename(file_path)
            print(f"  -> Scanning {filename}")
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            entities = re.finditer(r'\[ENTITY:\s*(\w+)\]', content)
            for match in entities:
                entity_name = match.group(1)
                start_pos = match.end()
                class_match = re.search(r'\[CLASS:\s*([\w<>]+)\]', content[start_pos:])
                entity_type = "Unknown"
                if class_match:
                    entity_type = class_match.group(1)
                effect_match = re.search(r'\[EFFECT:\s*(\w+)\]', content[start_pos:])
                entity_effect = None
                if effect_match:
                    entity_effect = effect_match.group(1)
                
                # Parse Properties
                props = {}
                prop_block_match = re.search(r'\[PROPERTIES:\s*(.*?)\]', content[start_pos:], re.DOTALL)
                if prop_block_match:
                    block_content = prop_block_match.group(1)
                    euler_match = re.search(r'-\s*euler_characteristic:\s*Number\s*=\s*(-?\d+)', block_content)
                    if euler_match: props['euler_characteristic'] = int(euler_match.group(1))
                    dim_match = re.search(r'-\s*dimension:\s*Number\s*=\s*(\d+)', block_content)
                    if dim_match: props['dimension'] = int(dim_match.group(1))
                    orient_match = re.search(r'-\s*is_orientable:\s*Boolean\s*=\s*(true|false)', block_content)
                    if orient_match: props['is_orientable'] = (orient_match.group(1) == 'true')
                    fg_match = re.search(r'-\s*fundamental_group:\s*String\s*=\s*"(.*?)"', block_content)
                    if fg_match: props['fundamental_group'] = fg_match.group(1)

                # Parse Input/Output for Mappings
                source_type = None
                target_type = None
                input_match = re.search(r'\[INPUT:\s*([\w\*<>\s]+)\]', content[start_pos:])
                if input_match: source_type = input_match.group(1).strip()
                output_match = re.search(r'\[OUTPUT:\s*([\w\*<>\s]+)\]', content[start_pos:])
                if output_match: target_type = output_match.group(1).strip()

                print(f"    + Registering {entity_name} ({entity_type}) [Effect: {entity_effect}] [Props: {props}] [Sig: {source_type} -> {target_type}]")
                definitions[entity_name] = {
                    'type': 'ExternalEntity', 
                    'class': entity_type, 
                    'effect': entity_effect,
                    'properties': props,
                    'origin_file': filename,
                    'source': source_type,
                    'target': target_type,
                    'function_name': re.search(r'\[FUNCTION:\s*(\w+)\]', content[start_pos:]).group(1) if re.search(r'\[FUNCTION:\s*(\w+)\]', content[start_pos:]) else None
                }
                func_name = definitions[entity_name]['function_name']
                if func_name: definitions[func_name] = definitions[entity_name]

        return definitions

# --- Semantic Analyzer (Type Checker) ---

class SemanticError(Exception):
    pass

class SemanticAnalyzer:
    def __init__(self, stdlib_path: str = "TOPOS-Xi/src/stdlib"):
        self.symbol_table = {}
        loader = StdLibLoader(stdlib_path)
        self.stdlib_symbols = loader.load_definitions()
        self.symbol_table.update(self.stdlib_symbols)

    def visit(self, node: ASTNode):
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node)

    def generic_visit(self, node: ASTNode):
        raise NotImplementedError(f'No visit_{type(node).__name__} method')

    def visit_list(self, nodes: List[ASTNode]):
        for node in nodes: self.visit(node)

    def visit_SpaceDef(self, node: SpaceDef):
        print(f"[Check] Analyzing Space: {node.name}")
        self.symbol_table[node.name] = {'type': 'Space', 'properties': {}}
        for prop in node.properties:
            self.visit_PropertyDef(prop, space_name=node.name)
        for member in node.members: self.visit(member)

    def visit_PropertyDef(self, node: PropertyDef, space_name: str):
        print(f"  [Property] {node.name}: {node.type}")
        self.symbol_table[space_name]['properties'][node.name] = node.type
        if node.default is not None:
            self._verify_assignment(node.type, node.default)

    def visit_MappingDef(self, node: MappingDef):
        path_str = ' -> '.join(self._step_to_str(s) for s in node.path)
        type_str = f": {node.source} -> {node.target}" if node.source else ""
        print(f"  [Mapping] {node.name}{type_str}: {path_str}")
        self.symbol_table[node.name] = {
            'type': 'Mapping', 'node': node, 'properties': {},
            'source': node.source, 'target': node.target
        }
        for prop in node.properties: self.visit_PropertyDef(prop, space_name=node.name)
        scope = {p.name: p.type for p in node.params} if node.params else {}
        self.analyze_path(node.name, node.path, node.source, node.target, scope)

    def visit_LambdaMapping(self, node: LambdaMapping):
        print(f"    [Lambda] Analyzing anonymous mapping...")
        scope = {p.name: p.type for p in node.params}
        self.analyze_path("anonymous", node.path, None, None, scope)

    def visit_MatchExpr(self, node: MatchExpr):
        print(f"    [Match] Analyzing branching logic on '{node.target}'...")
        for case in node.cases: self.analyze_path(f"case {case.value}", case.path, None, None)

    def _step_to_str(self, step):
        if isinstance(step, MappingCall): return step.name
        if isinstance(step, LambdaMapping): return "(lambda)"
        if isinstance(step, MatchExpr): return f"match({step.target})"
        return str(step)

    def _resolve_type_properties(self, type_expr: TypeExpr, scope: Dict[str, TypeExpr] = None) -> Dict[str, Any]:
        if not type_expr: return {}
        if scope and type_expr.name in scope: return self._resolve_type_properties(scope[type_expr.name], scope)
        if type_expr.name == '*':
            lp, rp = self._resolve_type_properties(type_expr.params[0], scope), self._resolve_type_properties(type_expr.params[1], scope)
            props = {}
            if lp.get('dimension') is not None and rp.get('dimension') is not None: props['dimension'] = lp['dimension'] + rp['dimension']
            if lp.get('euler_characteristic') is not None and rp.get('euler_characteristic') is not None: props['euler_characteristic'] = lp['euler_characteristic'] * rp['euler_characteristic']
            if lp.get('is_orientable') is not None and rp.get('is_orientable') is not None: props['is_orientable'] = lp['is_orientable'] and rp['is_orientable']
            lfg, rfg = lp.get('fundamental_group'), rp.get('fundamental_group')
            if lfg and rfg:
                if lfg == '0': props['fundamental_group'] = rfg
                elif rfg == '0': props['fundamental_group'] = lfg
                else: props['fundamental_group'] = f"{lfg} x {rfg}"
            return props
        return self.symbol_table.get(type_expr.name, {}).get('properties', {})

    def _is_equivalent(self, t1: TypeExpr, t2: TypeExpr, scope: Dict[str, TypeExpr]) -> bool:
        if t1 is None and t2 is None: return True
        if t1 is None or t2 is None: return False
        lhs = scope[t1.name] if t1.name in scope else t1
        rhs = scope[t2.name] if t2.name in scope else t2
        if lhs.name != rhs.name: return False
        if (lhs.params is None) != (rhs.params is None): return False
        if lhs.params:
            if len(lhs.params) != len(rhs.params): return False
            for p1, p2 in zip(lhs.params, rhs.params):
                if not self._is_equivalent(p1, p2, scope): return False
        return True

    def _infer_type(self, val: Any, scope: Dict[str, TypeExpr]) -> TypeExpr:
        if isinstance(val, MappingCall):
            info = self.symbol_table.get(val.name, {})
            target = info.get('target')
            if isinstance(target, str):
                if '*' in target: return TypeExpr('*', [TypeExpr(p.strip(), []) for p in target.split('*')])
                return TypeExpr(target, [])
            return target if isinstance(target, TypeExpr) else TypeExpr("Unknown", [])
        if isinstance(val, str):
            if val in scope: return scope[val]
            if val in self.symbol_table: return TypeExpr(val, [])
            try: float(val); return TypeExpr("Number", [])
            except ValueError: return TypeExpr("String", [])
        return TypeExpr("Unknown", [])

    def analyze_path(self, context_name: str, path: List[Any], expected_src: Optional[TypeExpr], expected_dst: Optional[TypeExpr], scope: Dict[str, TypeExpr] = None):
        if scope is None: scope = {}
        if not path: raise SemanticError(f"Path in '{context_name}' must have at least one step.")
        cursor_type = expected_src
        for i in range(len(path)):
            step = path[i]
            if isinstance(step, (LambdaMapping, MatchExpr)):
                if isinstance(step, LambdaMapping): self.visit_LambdaMapping(step)
                elif isinstance(step, MatchExpr):
                    print(f"    [Match] Analyzing branching logic on '{step.target}'...")
                    for case in step.cases:
                        self.analyze_path(f"case {case.value}", case.path, cursor_type, None, scope)
                # After match, the resulting type is ambiguous without union types.
                # For now, we set to None to require explicit initialization if path continues.
                cursor_type = None
                continue
            step_name = step.name if isinstance(step, MappingCall) else step
            if step_name in ('fst', 'snd'):
                rc = scope[cursor_type.name] if cursor_type and cursor_type.name in scope else cursor_type
                if not rc or rc.name != '*': raise SemanticError(f"Projection Error in '{context_name}': '{step_name}' requires a Product Type source, got {cursor_type}")
                cursor_type = rc.params[0] if step_name == 'fst' else rc.params[1]
                continue
            if step_name == 'pair':
                if isinstance(step, MappingCall) and len(step.args) == 2:
                    cursor_type = TypeExpr('*', [self._infer_type(step.args[0], scope), self._infer_type(step.args[1], scope)])
                    continue
                else: raise SemanticError(f"Pair Error in '{context_name}': 'pair' requires 2 arguments")
            step_info = self.symbol_table.get(step_name, {})
            ss, st = step_info.get('source'), step_info.get('target')
            if isinstance(ss, str): ss = TypeExpr('*', [TypeExpr(p.strip(), []) for p in ss.split('*')]) if '*' in ss else TypeExpr(ss, [])
            if isinstance(st, str): st = TypeExpr('*', [TypeExpr(p.strip(), []) for p in st.split('*')]) if '*' in st else TypeExpr(st, [])
            if step_info.get('type') in ('Space', 'ExternalEntity', 'Manifold'):
                 if not ss: ss = TypeExpr(step_name, [])
                 if not st: st = TypeExpr(step_name, [])
            if ss and cursor_type and not self._is_equivalent(cursor_type, ss, scope) and step_name != 'init':
                raise SemanticError(f"Composition Error in '{context_name}': Step '{step_name}' expects source {ss}, but previous output was {cursor_type}")
            if st: cursor_type = st
            elif step_name == 'init':
                if expected_src and expected_dst:
                    sp, dp = self._resolve_type_properties(expected_src, scope), self._resolve_type_properties(expected_dst, scope)
                    se, de = sp.get('euler_characteristic'), dp.get('euler_characteristic')
                    if se is not None and de is not None and se != de: raise SemanticError(f"Topological Violation in '{context_name}': {expected_src}(X={se}) -> {expected_dst}(X={de})")
                    so, do = sp.get('is_orientable'), dp.get('is_orientable')
                    if so is not None and do is not None and so != do: raise SemanticError(f"Orientation Violation in '{context_name}': {expected_src}(Orientable={so}) -> {expected_dst}(Orientable={do})")
                    sf, df = sp.get('fundamental_group'), dp.get('fundamental_group')
                    if sf and df and sf != df: raise SemanticError(f"Homotopy Violation in '{context_name}': {expected_src}(pi1={sf}) -> {expected_dst}(pi1={df})")
                cursor_type = expected_dst
        if expected_dst and cursor_type and not self._is_equivalent(cursor_type, expected_dst, scope) and not any(isinstance(s, str) and s == 'init' for s in path):
            raise SemanticError(f"Morphism Error in '{context_name}': Expected final target {expected_dst}, but path ends with {cursor_type}")
        for step in path:
            if isinstance(step, (LambdaMapping, MatchExpr)): continue
            sn = step.name if isinstance(step, MappingCall) else step
            if sn in self.symbol_table and self.symbol_table[sn].get('effect') == 'Collapse_Wavefunction': print(f"      [WARNING] Step '{sn}' causes Wavefunction Collapse.")

    def _verify_assignment(self, target_type: TypeExpr, value: str):
        tn = target_type.name
        if tn == 'Boolean' and value not in ('true', 'false'): raise SemanticError(f"Type Mismatch: Expected Boolean, got '{value}'")
        elif tn == 'Number':
            try: float(value)
            except ValueError: raise SemanticError(f"Type Mismatch: Expected Number, got '{value}'")
        elif tn in ('String', '*'): pass
        elif tn in self.symbol_table: print(f"    [TypeCheck] Verified existence of type '{tn}' in Symbol Table.")
        elif tn in ('Topology', 'Quantum'): pass
        else: raise SemanticError(f"Unknown Type: '{tn}'.")

# --- Quantum Simulator ---

class QuantumState:
    def __init__(self, alpha: complex = 1.0, beta: complex = 0.0):
        self.alpha, self.beta, self.last_result = alpha, beta, None
    def measure(self) -> str:
        self.last_result = '0' if random.random() < abs(self.alpha)**2 else '1'
        if self.last_result == '0': self.alpha, self.beta = 1.0, 0.0
        else: self.alpha, self.beta = 0.0, 1.0
        return self.last_result
    def __repr__(self):
        return self.last_result if self.last_result else f"{self.alpha:.2f}|0> + {self.beta:.2f}|1>"
    def __str__(self):
        return self.__repr__()

# --- Interpreter ---

@dataclass
class Pair:
    left: Any
    right: Any
    def __repr__(self):
        return f"({self.left}, {self.right})"

class Interpreter:
    def __init__(self, symbol_table: Dict[str, Any]):
        self.symbol_table, self.runtime_state, self.mappings, self.max_depth, self.scope_stack = symbol_table, {}, {}, 10, [{}]
    def run(self, ast: List[ASTNode], entry_point: str = 'main'):
        print("\n--- 4. Execution (Runtime) ---")
        for node in ast:
            if isinstance(node, SpaceDef):
                for member in node.members:
                    if isinstance(member, MappingDef): self.mappings[member.name] = member
        for node in ast: self.execute(node)
        if entry_point in self.mappings:
            print(f"\n[Entry] Calling '{entry_point}'...")
            return self.execute_MappingDef(self.mappings[entry_point])
        return None
    def execute(self, node: ASTNode):
        m = getattr(self, f'execute_{type(node).__name__}', lambda n: None)
        return m(node)
    def _resolve_var(self, name: str):
        for scope in reversed(self.scope_stack):
            if name in scope: return scope[name]
        for sd in self.runtime_state.values():
            if name in sd['properties']: return sd['properties'][name]
        return None
    def _evaluate_arg(self, arg: Any) -> Any:
        if isinstance(arg, MappingCall):
            if arg.name == 'pair':
                v1 = self._evaluate_arg(arg.args[0])
                v2 = self._evaluate_arg(arg.args[1])
                return Pair(v1, v2)
            if arg.name == 'fst':
                v = self._evaluate_arg(arg.args[0])
                return v.left if isinstance(v, Pair) else v
            if arg.name == 'snd':
                v = self._evaluate_arg(arg.args[0])
                return v.right if isinstance(v, Pair) else v
            if arg.name in self.symbol_table:
                info = self.symbol_table[arg.name]
                if info.get('type') == 'ExternalEntity':
                    call_args = [self._evaluate_arg(a) for a in arg.args]
                    if arg.name == 'token': 
                        if len(call_args) == 2:
                            return f"TOKEN({call_args[0]}: {call_args[1]})"
                        return f"TOKEN({call_args[0]})"
                    if arg.name == 'init_pos':
                        if len(call_args) >= 3:
                            return {'line': call_args[0], 'column': call_args[1], 'offset': call_args[2]}
                        return {'line': 1, 'column': 0, 'offset': 0}

                    if arg.name == 'read_char':
                        s = str(call_args[0])
                        if s: return Pair(s[1:], s[0])
                        return Pair("", "")
                    if arg.name == 'advance_pos':
                        # Expects Pair(Position, String)
                        pos_pair = call_args[0]
                        if isinstance(pos_pair, Pair):
                            pos = pos_pair.left
                            char = str(pos_pair.right).strip('"')
                            
                            # Handle Position as Dict (Proper Space Instance)
                            if isinstance(pos, dict):
                                new_pos = pos.copy()
                                new_pos['offset'] = float(new_pos.get('offset', 0)) + 1
                                if char == '\\n' or char == '\n':
                                    new_pos['line'] = float(new_pos.get('line', 1)) + 1
                                    new_pos['column'] = 0 
                                else:
                                    new_pos['column'] = float(new_pos.get('column', 0)) + 1
                                return new_pos

                            # Fallback for Number (Legacy/Mock)
                            try:
                                return float(pos) + 1
                            except: pass
                        return 0
                    return call_args[0] if call_args else arg.name
            if arg.name in self.mappings: return self.execute_MappingDef(self.mappings[arg.name], [self._evaluate_arg(a) for a in arg.args])
            return arg.name
        if isinstance(arg, str):
            res = self._resolve_var(arg)
            return res if res is not None else arg
        return arg
    def execute_SpaceDef(self, node: SpaceDef):
        print(f"[Run] Entering Space: {node.name}")
        self.runtime_state[node.name] = {'properties': {}}
        for prop in node.properties: self.runtime_state[node.name]['properties'][prop.name] = self.execute(prop)
        for member in node.members:
            if not isinstance(member, MappingDef): self.execute(member)
    def execute_PropertyDef(self, node: PropertyDef):
        val = node.default
        if isinstance(val, MappingCall):
            val = self._evaluate_arg(val)
        
        if node.type.name == 'Qubit':
            if val in ('"0"', '0'): val = QuantumState(1.0, 0.0)
            elif val in ('"1"', '1'): val = QuantumState(0.0, 1.0)
            else: val = QuantumState(complex(1/math.sqrt(2)), complex(1/math.sqrt(2)))
        elif node.type.name == 'Number':
            try: val = float(val)
            except: pass
        elif node.type.name == 'Boolean': val = (val == 'true')
        elif node.type.name == 'String': val = str(val).strip('"')
        print(f"  [Init] Property '{node.name}' set to: {val}")
        return val
    def execute_MappingDef(self, node: MappingDef, args: List[Any] = None, depth: int = 0):
        if depth > self.max_depth: raise RuntimeError(f"Recursion depth exceeded: {depth}")
        print(f"{ '  '*(depth+1)}[Exec] Mapping '{node.name}' (Depth {depth}):")
        ns = {}
        if args and node.params:
            for i, p in enumerate(node.params):
                if i < len(args): ns[p.name] = args[i]
        self.scope_stack.append(ns)
        for prop in node.properties: ns[prop.name] = self.execute(prop)
        res = self.execute_path(node.path, depth, args[0] if args else None)
        self.scope_stack.pop()
        return res
    def execute_path(self, path: List[Any], depth: int, cv: Any = None):
        ind = "  " * (depth + 1)
        for step in path:
            if isinstance(step, LambdaMapping):
                self.scope_stack.append({})
                cv = self.execute_path(step.path, depth + 1, cv)
                self.scope_stack.pop()
            elif isinstance(step, MatchExpr):
                val = self._resolve_var(step.target)
                if val is None:
                    # Support matching on projections of current value
                    if step.target == 'fst' and isinstance(cv, Pair): val = cv.left
                    elif step.target == 'snd' and isinstance(cv, Pair): val = cv.right
                
                tv = str(val).strip('"')
                for case in step.cases:
                    cv_str = str(case.value).strip('"')
                    if tv == cv_str or cv_str == '_':
                        print(f"{ind}    [Match] Case '{cv_str}' matched!")
                        cv = self.execute_path(case.path, depth + 1, cv)
                        break
            else:
                sn = step.name if isinstance(step, MappingCall) else step
                
                # Try to resolve as variable load first
                resolved_var = self._resolve_var(sn)
                if resolved_var is not None and not isinstance(step, MappingCall):
                    cv = resolved_var
                elif sn == 'fst':
                    if isinstance(cv, Pair): cv = cv.left
                    else: print(f"{ind}  [Error] fst on non-pair: {cv}")
                elif sn == 'snd':
                    if isinstance(cv, Pair): cv = cv.right
                    else: print(f"{ind}  [Error] snd on non-pair: {cv}")
                elif sn == 'pair': cv = Pair(self._evaluate_arg(step.args[0]), self._evaluate_arg(step.args[1]))
                elif sn == 'Measurement':
                    if isinstance(cv, QuantumState): cv = cv.measure()
                    else:
                        for sd in self.runtime_state.values():
                            for pv in sd['properties'].values():
                                if isinstance(pv, QuantumState): pv.measure()
                elif sn in self.mappings:
                    ca = [self._evaluate_arg(a) for a in step.args] if isinstance(step, MappingCall) else ([cv] if cv is not None else [])
                    cv = self.execute_MappingDef(self.mappings[sn], ca, depth + 1)
                elif sn in self.symbol_table and self.symbol_table[sn].get('type') == 'ExternalEntity':
                    # Execute external morphism with current value or explicit args
                    call_node = step if isinstance(step, MappingCall) else MappingCall(sn, [cv] if cv is not None else [])
                    cv = self._evaluate_arg(call_node)
                
                print(f"{ind}  -> Step: {sn} (out: {cv})")
        return cv

# --- Parser ---

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens, self.pos = tokens, 0
    def peek(self) -> Token: return self.tokens[self.pos]
    def consume(self, et: Optional[TokenType] = None, ev: Optional[str] = None) -> Token:
        t = self.peek()
        if et and t.type != et: raise SyntaxError(f'Expected {et}, got {t.type} at {t.line}:{t.column}')
        if ev and t.value != ev: raise SyntaxError(f'Expected "{ev}", got "{t.value}" at {t.line}:{t.column}')
        self.pos += 1
        return t
    def parse_program(self) -> List[SpaceDef]:
        ss = []
        while self.peek().type != TokenType.EOF: ss.append(self.parse_space())
        return ss
    def parse_space(self) -> SpaceDef:
        self.consume(TokenType.KEYWORD, 'space')
        n = self.consume(TokenType.IDENTIFIER).value
        self.consume(TokenType.DELIMITER, '{')
        ps, ms = [], []
        if self.peek().value == 'properties': ps = self.parse_properties()
        while self.peek().value != '}':
            if self.peek().value == 'mapping': ms.append(self.parse_mapping())
            else: self.pos += 1
        self.consume(TokenType.DELIMITER, '}')
        return SpaceDef(n, ps, ms)
    def parse_properties(self) -> List[PropertyDef]:
        self.consume(TokenType.KEYWORD, 'properties')
        self.consume(TokenType.DELIMITER, '{')
        ps = []
        while self.peek().value != '}':
            n = self.consume(TokenType.IDENTIFIER).value
            self.consume(TokenType.DELIMITER, ':')
            t, d = self.parse_type(), None
            if self.peek().value == '=':
                self.consume(TokenType.OPERATOR, '=')
                d = self.parse_value_or_call()
            ps.append(PropertyDef(n, t, d))
        self.consume(TokenType.DELIMITER, '}')
        return ps
    def parse_type(self) -> TypeExpr:
        l = self.parse_type_atom()
        if self.peek().value == '*':
            self.consume(TokenType.OPERATOR, '*')
            return TypeExpr('*', [l, self.parse_type()])
        return l
    def parse_type_atom(self) -> TypeExpr:
        if self.peek().value == '(': 
            self.consume(TokenType.DELIMITER, '(')
            t = self.parse_type()
            self.consume(TokenType.DELIMITER, ')')
            return t
        n = self.consume().value
        ps = []
        if self.peek().value == '<':
            self.consume(TokenType.OPERATOR, '<')
            while True:
                ps.append(self.parse_type())
                if self.peek().value == '>': break
                self.consume(TokenType.DELIMITER, ',')
            self.consume(TokenType.OPERATOR, '>')
        return TypeExpr(n, ps)
    def parse_params(self) -> List[Parameter]:
        self.consume(TokenType.DELIMITER, '(')
        ps = []
        while self.peek().value != ')':
            n = self.consume(TokenType.IDENTIFIER).value
            self.consume(TokenType.DELIMITER, ':')
            ps.append(Parameter(n, self.parse_type()))
            if self.peek().value == ',': self.consume(TokenType.DELIMITER, ',')
        self.consume(TokenType.DELIMITER, ')')
        return ps
    def parse_value_or_call(self) -> Any:
        t = self.peek()
        if t.type == TokenType.IDENTIFIER:
            n = self.consume(TokenType.IDENTIFIER).value
            if self.peek().value == '(': 
                self.consume(TokenType.DELIMITER, '(')
                args = []
                while self.peek().value != ')':
                    args.append(self.parse_value_or_call())
                    if self.peek().value == ',': self.consume(TokenType.DELIMITER, ',')
                self.consume(TokenType.DELIMITER, ')')
                return MappingCall(n, args)
            return n
        if t.type in (TokenType.STRING, TokenType.NUMBER): return self.consume().value
        raise SyntaxError(f"Unexpected token: {t}")
    def parse_path_content(self) -> List[Any]:
        self.consume(TokenType.KEYWORD, 'path')
        self.consume(TokenType.DELIMITER, '{')
        path = []
        while self.peek().value != '}':
            if self.peek().value == '(': 
                lp = self.parse_params()
                self.consume(TokenType.DELIMITER, '{')
                lpc = self.parse_path_content()
                self.consume(TokenType.DELIMITER, '}')
                path.append(LambdaMapping(lp, lpc))
            elif self.peek().value == 'match':
                self.consume(TokenType.KEYWORD, 'match')
                self.consume(TokenType.DELIMITER, '(')
                t = self.consume(TokenType.IDENTIFIER).value
                self.consume(TokenType.DELIMITER, ')')
                self.consume(TokenType.DELIMITER, '{')
                cs = []
                while self.peek().value != '}':
                    v = self.consume().value
                    self.consume(TokenType.OPERATOR, '->')
                    self.consume(TokenType.DELIMITER, '{')
                    cpc = self.parse_path_content()
                    self.consume(TokenType.DELIMITER, '}')
                    cs.append(MatchCase(v, cpc))
                self.consume(TokenType.DELIMITER, '}')
                path.append(MatchExpr(t, cs))
            else: path.append(self.parse_value_or_call())
            if self.peek().value == '->': self.consume(TokenType.OPERATOR, '->')
        self.consume(TokenType.DELIMITER, '}')
        return path
    def parse_mapping(self) -> MappingDef:
        self.consume(TokenType.KEYWORD, 'mapping')
        n, ps = self.consume(TokenType.IDENTIFIER).value, self.parse_params()
        ss, st = None, None
        if self.peek().value == ':':
            self.consume(TokenType.DELIMITER, ':')
            ss = self.parse_type()
            self.consume(TokenType.OPERATOR, '->')
            st = self.parse_type()
        elif self.peek().value == '->':
            self.consume(TokenType.OPERATOR, '->')
            st = self.parse_type()
        self.consume(TokenType.DELIMITER, '{')
        props = self.parse_properties() if self.peek().value == 'properties' else []
        path = self.parse_path_content()
        self.consume(TokenType.DELIMITER, '}')
        return MappingDef(n, ps, props, path, ss, st)
