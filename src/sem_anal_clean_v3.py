    def _infer_type(self, val: Any, scope: Dict[str, TypeExpr]) -> TypeExpr:
        if isinstance(val, MappingCall):
            info = self.symbol_table.get(val.name, {})
            target = info.get('target')
            if isinstance(target, str):
                if '*' in target:
                    parts = target.split('*')
                    return TypeExpr('*', [TypeExpr(p.strip(), []) for p in parts])
                return TypeExpr(target, [])
            elif isinstance(target, TypeExpr):
                return target
            return TypeExpr("Unknown", [])
        elif isinstance(val, str):
            if val in scope:
                return scope[val]
            if val in self.symbol_table:
                return TypeExpr(val, [])
            try:
                float(val)
                return TypeExpr("Number", [])
            except ValueError:
                return TypeExpr("String", [])
        return TypeExpr("Unknown", [])

    def analyze_path(self, context_name: str, path: List[Any], 
                     expected_src: Optional[TypeExpr], expected_dst: Optional[TypeExpr],
                     scope: Dict[str, TypeExpr] = None):
        if scope is None: scope = {}
        if not path:
            raise SemanticError(f"Path in '{context_name}' must have at least one step.")
            
        cursor_type = expected_src
        
        for i in range(len(path)):
            step = path[i]
            if isinstance(step, (LambdaMapping, MatchExpr)):
                if isinstance(step, LambdaMapping): self.visit_LambdaMapping(step)
                elif isinstance(step, MatchExpr): self.visit_MatchExpr(step)
                cursor_type = None 
                continue
            
            step_name = step.name if isinstance(step, MappingCall) else step
            s_dst_obj = None
            
            if step_name in ('fst', 'snd'):
                resolved_cursor = cursor_type
                if resolved_cursor and resolved_cursor.name in scope:
                    resolved_cursor = scope[resolved_cursor.name]

                if not resolved_cursor or resolved_cursor.name != '*':
                     raise SemanticError(f"Projection Error in '{context_name}': '{step_name}' requires a Product Type source, got {cursor_type}")
                cursor_type = resolved_cursor.params[0] if step_name == 'fst' else resolved_cursor.params[1]
                continue
            
            if step_name == 'pair':
                if isinstance(step, MappingCall) and len(step.args) == 2:
                    t1 = self._infer_type(step.args[0], scope)
                    t2 = self._infer_type(step.args[1], scope)
                    cursor_type = TypeExpr('*', [t1, t2])
                    continue
                else: raise SemanticError(f"Pair Error in '{context_name}': 'pair' requires 2 arguments")
            
            step_info = self.symbol_table.get(step_name, {})
            s_src_obj = step_info.get('source')
            s_dst_obj = step_info.get('target')
            
            if isinstance(s_src_obj, str):
                if '*' in s_src_obj:
                    parts = s_src_obj.split('*')
                    s_src_obj = TypeExpr('*', [TypeExpr(p.strip(), []) for p in parts])
                else: s_src_obj = TypeExpr(s_src_obj, [])
            
            if isinstance(s_dst_obj, str):
                if '*' in s_dst_obj:
                    parts = s_dst_obj.split('*')
                    s_dst_obj = TypeExpr('*', [TypeExpr(p.strip(), []) for p in parts])
                else: s_dst_obj = TypeExpr(s_dst_obj, [])

            if step_info.get('type') in ('Space', 'ExternalEntity', 'Manifold'):
                 if not s_src_obj: s_src_obj = TypeExpr(step_name, [])
                 if not s_dst_obj: s_dst_obj = TypeExpr(step_name, [])

            if s_src_obj and cursor_type:
                if not self._is_equivalent(cursor_type, s_src_obj, scope):
                    if step_name != 'init':
                        raise SemanticError(f"Composition Error in '{context_name}': Step '{step_name}' expects source {s_src_obj}, but previous output was {cursor_type}")
            
            if s_dst_obj:
                cursor_type = s_dst_obj 
            elif step_name == 'init':
                if expected_src and expected_dst:
                    src_props = self._resolve_type_properties(expected_src, scope)
                    dst_props = self._resolve_type_properties(expected_dst, scope)
                    s_euler, d_euler = src_props.get('euler_characteristic'), dst_props.get('euler_characteristic')
                    if s_euler is not None and d_euler is not None and s_euler != d_euler:
                        raise SemanticError(f"Topological Violation in '{context_name}': Cannot initialize mapping {expected_src} (Euler={s_euler}) -> {expected_dst} (Euler={d_euler})")
                    s_orient, d_orient = src_props.get('is_orientable'), dst_props.get('is_orientable')
                    if s_orient is not None and d_orient is not None and s_orient != d_orient:
                        raise SemanticError(f"Orientation Violation in '{context_name}': Cannot initialize mapping {expected_src} (Orientable={s_orient}) -> {expected_dst} (Orientable={d_orient})")
                    s_fg, d_fg = src_props.get('fundamental_group'), dst_props.get('fundamental_group')
                    if s_fg and d_fg and s_fg != d_fg:
                        raise SemanticError(f"Homotopy Violation in '{context_name}': Cannot initialize mapping {expected_src} (pi1={s_fg}) -> {expected_dst} (pi1={d_fg})")
                cursor_type = expected_dst

        if expected_dst and cursor_type:
            if not self._is_equivalent(cursor_type, expected_dst, scope):
                if not any(isinstance(s, str) and s == 'init' for s in path):
                    raise SemanticError(f"Morphism Error in '{context_name}': Expected final target {expected_dst}, but path ends with {cursor_type}")

        for step in path:
            if isinstance(step, (LambdaMapping, MatchExpr)): continue
            step_name = step.name if isinstance(step, MappingCall) else step
            if step_name in self.symbol_table:
                effect = self.symbol_table[step_name].get('effect')
                if effect == 'Collapse_Wavefunction': print(f"      [WARNING] Step '{step_name}' causes '{effect}'.")
