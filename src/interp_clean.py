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
                else:
                    call_args = [current_val] if current_val is not None else []
                current_val = self.execute_MappingDef(self.mappings[step_name], call_args, depth + 1)
            elif step_name == 'init':
                pass
        
        return current_val
