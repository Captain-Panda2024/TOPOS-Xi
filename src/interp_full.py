    def _resolve_var(self, name: str):
        for scope in reversed(self.scope_stack):
            if name in scope: return scope[name]
        for space_name, space_data in self.runtime_state.items():
            if name in space_data['properties']: return space_data['properties'][name]
        return None

    def execute_SpaceDef(self, node: SpaceDef):
        print(f"[Run] Entering Space: {node.name}")
        self.runtime_state[node.name] = {'properties': {}}
        for prop in node.properties:
            val = self.execute(prop)
            self.runtime_state[node.name]['properties'][prop.name] = val
        for member in node.members:
            if not isinstance(member, MappingDef): self.execute(member)

    def execute_PropertyDef(self, node: PropertyDef):
        val = node.default
        if isinstance(val, MappingCall):
            if val.name == 'pair':
                v1 = self._resolve_var(val.args[0])
                v2 = self._resolve_var(val.args[1])
                val = Pair(v1 if v1 is not None else val.args[0], 
                           v2 if v2 is not None else val.args[1])
        
        if node.type.name == 'Qubit':
            if val in ('"0"', '0'): val = QuantumState(1.0, 0.0)
            elif val in ('"1"', '1'): val = QuantumState(0.0, 1.0)
            else: val = QuantumState(complex(1/math.sqrt(2)), complex(1/math.sqrt(2)))
        elif node.type.name == 'Number':
            try: val = float(val) if val is not None else 0.0
            except: pass
        elif node.type.name == 'Boolean': val = (val == 'true') if val is not None else False
        elif node.type.name == 'String': val = val if val is not None else ""
        print(f"  [Init] Property '{node.name}' set to: {val}")
        return val

    def execute_MappingDef(self, node: MappingDef, args: List[Any] = None, depth: int = 0):
        if depth > self.max_depth: raise RuntimeError(f"Recursion depth exceeded in mapping '{node.name}'")
        indent = "  " * (depth + 1)
        print(f"{indent}[Exec] Mapping '{node.name}':")
        new_scope = {}
        if args and node.params:
            for i, param in enumerate(node.params):
                if i < len(args): new_scope[param.name] = args[i]
        self.scope_stack.append(new_scope)
        for prop in node.properties:
            val = self.execute(prop)
            new_scope[prop.name] = val
        initial_val = args[0] if args else None
        result = self.execute_path(node.path, depth, initial_val)
        self.scope_stack.pop()
        return result

    def _evaluate_arg(self, arg: Any) -> Any:
        if isinstance(arg, MappingCall):
            if arg.name == 'pair':
                v1 = self._evaluate_arg(arg.args[0])
                v2 = self._evaluate_arg(arg.args[1])
                return Pair(v1, v2)
            if arg.name in self.symbol_table:
                info = self.symbol_table[arg.name]
                if info.get('type') == 'ExternalEntity':
                    call_args = [self._evaluate_arg(a) for a in arg.args]
                    if arg.name == 'token': return f"TOKEN({call_args[0]})"
                    return call_args[0] if call_args else arg.name
            if arg.name in self.mappings:
                call_args = [self._evaluate_arg(a) for a in arg.args]
                return self.execute_MappingDef(self.mappings[arg.name], call_args)
            return arg.name
        return self._resolve_var(arg) if isinstance(arg, str) else arg

    def execute_path(self, path: List[Any], depth: int, current_val: Any = None):
        indent = "  " * (depth + 1)
        for step in path:
            if isinstance(step, LambdaMapping):
                print(f"{indent}  -> Step: [Lambda]")
                self.scope_stack.append({})
                current_val = self.execute_path(step.path, depth + 1, current_val)
                self.scope_stack.pop()
                continue
            if isinstance(step, MatchExpr):
                print(f"{indent}  -> Step: match({step.target})")
                val = self._resolve_var(step.target)
                if isinstance(val, QuantumState): val = str(val) 
                target_val = str(val).strip('"')
                for case in step.cases:
                    case_val = str(case.value).strip('"')
                    if target_val == case_val:
                        print(f"{indent}    [Match] Case '{case_val}' matched!")
                        current_val = self.execute_path(case.path, depth + 1, current_val)
                        break
                continue
            step_name = step.name if isinstance(step, MappingCall) else step
            if step_name == 'fst':
                if isinstance(current_val, Pair):
                    print(f"{indent}  -> Step: fst => {current_val.left}")
                    current_val = current_val.left
                else: print(f"{indent}  [Error] fst called on non-pair: {current_val}")
                continue
            elif step_name == 'snd':
                if isinstance(current_val, Pair):
                    print(f"{indent}  -> Step: snd => {current_val.right}")
                    current_val = current_val.right
                else: print(f"{indent}  [Error] snd called on non-pair: {current_val}")
                continue
            elif step_name == 'pair':
                if isinstance(step, MappingCall) and len(step.args) == 2:
                    v1 = self._evaluate_arg(step.args[0])
                    v2 = self._evaluate_arg(step.args[1])
                    current_val = Pair(v1, v2)
                    print(f"{indent}  -> Step: pair => {current_val}")
                else: print(f"{indent}  [Error] pair requires 2 arguments")
                continue
            print(f"{indent}  -> Step: {step_name} (in: {current_val})")
            if step_name == 'Measurement':
                if isinstance(current_val, QuantumState):
                    res = current_val.measure()
                    print(f"{indent}    [Quantum] Measured: |{res}>")
                    current_val = res
                else:
                    for space in self.runtime_state.values():
                        for prop_name, prop_val in space['properties'].items():
                            if isinstance(prop_val, QuantumState):
                                result = prop_val.measure()
                                print(f"{indent}    [Quantum] Space wavefunction collapsed! {prop_name} -> |{result}>")
                                current_val = result
            elif step_name in self.mappings:
                call_args = []
                if isinstance(step, MappingCall):
                    call_args = [self._evaluate_arg(a) for a in step.args]
                else: call_args = [current_val] if current_val is not None else []
                current_val = self.execute_MappingDef(self.mappings[step_name], call_args, depth + 1)
            elif step_name == 'init': pass
        return current_val
