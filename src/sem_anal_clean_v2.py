    def visit_MappingDef(self, node: MappingDef):
        path_str = ' -> '.join(self._step_to_str(s) for s in node.path)
        type_str = f": {node.source} -> {node.target}" if node.source else ""
        print(f"  [Mapping] {node.name}{type_str}: {path_str}")
        
        self.symbol_table[node.name] = {
            'type': 'Mapping', 
            'node': node, 
            'properties': {},
            'source': node.source, 
            'target': node.target
        }
        
        for prop in node.properties:
            self.visit_PropertyDef(prop, space_name=node.name)
            
        scope = {p.name: p.type for p in node.params} if node.params else {}
        self.analyze_path(node.name, node.path, node.source, node.target, scope)

    def visit_LambdaMapping(self, node: LambdaMapping):
        print(f"    [Lambda] Analyzing anonymous mapping...")
        scope = {p.name: p.type for p in node.params}
        self.analyze_path("anonymous", node.path, None, None, scope)

    def visit_MatchExpr(self, node: MatchExpr):
        print(f"    [Match] Analyzing branching logic on '{node.target}'...")
        for case in node.cases:
            self.analyze_path(f"case {case.value}", case.path, None, None)

    def _step_to_str(self, step):
        if isinstance(step, MappingCall): return step.name
        if isinstance(step, LambdaMapping): return "(lambda)"
        if isinstance(step, MatchExpr): return f"match({step.target})"
        return str(step)

    def _get_space_properties(self, type_name: str) -> Dict[str, Any]:
        pass

    def _resolve_type_properties(self, type_expr: TypeExpr, scope: Dict[str, TypeExpr] = None) -> Dict[str, Any]:
        if not type_expr: return {}
        
        if scope and type_expr.name in scope:
            return self._resolve_type_properties(scope[type_expr.name], scope)

        if type_expr.name == '*':
            left_props = self._resolve_type_properties(type_expr.params[0], scope)
            right_props = self._resolve_type_properties(type_expr.params[1], scope)
            
            props = {}
            l_dim = left_props.get('dimension')
            r_dim = right_props.get('dimension')
            if l_dim is not None and r_dim is not None:
                props['dimension'] = l_dim + r_dim
            
            l_euler = left_props.get('euler_characteristic')
            r_euler = right_props.get('euler_characteristic')
            if l_euler is not None and r_euler is not None:
                props['euler_characteristic'] = l_euler * r_euler
                
            l_orient = left_props.get('is_orientable')
            r_orient = right_props.get('is_orientable')
            if l_orient is not None and r_orient is not None:
                props['is_orientable'] = l_orient and r_orient

            l_fg = left_props.get('fundamental_group')
            r_fg = right_props.get('fundamental_group')
            if l_fg and r_fg:
                if l_fg == '0': props['fundamental_group'] = r_fg
                elif r_fg == '0': props['fundamental_group'] = l_fg
                else: props['fundamental_group'] = f"{l_fg} x {r_fg}"
            return props
            
        return self.symbol_table.get(type_expr.name, {}).get('properties', {})

    def _is_equivalent(self, t1: TypeExpr, t2: TypeExpr, scope: Dict[str, TypeExpr]) -> bool:
        if t1 is None and t2 is None: return True
        if t1 is None or t2 is None: return False
        
        lhs = t1
        if lhs.name in scope: lhs = scope[lhs.name]
        
        rhs = t2
        if rhs.name in scope: rhs = scope[rhs.name]
        
        if lhs.name != rhs.name: return False
        if (lhs.params is None) != (rhs.params is None): return False
        if lhs.params:
            if len(lhs.params) != len(rhs.params): return False
            for p1, p2 in zip(lhs.params, rhs.params):
                if not self._is_equivalent(p1, p2, scope): return False
        return True

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
                # dereference cursor_type if alias
                resolved_cursor = cursor_type
                if resolved_cursor and resolved_cursor.name in scope:
                    resolved_cursor = scope[resolved_cursor.name]

                if not resolved_cursor or resolved_cursor.name != '*':
                     raise SemanticError(f"Projection Error in '{context_name}': '{step_name}' requires a Product Type source, got {cursor_type}")
                cursor_type = resolved_cursor.params[0] if step_name == 'fst' else resolved_cursor.params[1]
                continue
            
            if step_name == 'pair':
                if isinstance(step, MappingCall) and len(step.args) == 2:
                    t1, t2 = TypeExpr(step.args[0], []), TypeExpr(step.args[1], [])
                    cursor_type = TypeExpr('*', [t1, t2])
                    continue
                else: raise SemanticError(f"Pair Error in '{context_name}': 'pair' requires 2 arguments")
            
            step_info = self.symbol_table.get(step_name, {})
            s_src_obj = step_info.get('source')
            s_dst_obj = step_info.get('target')

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

        # 2. Effects & Continuity (simplified for brevity as main logic is above)
        for step in path:
            if isinstance(step, (LambdaMapping, MatchExpr)): continue
            step_name = step.name if isinstance(step, MappingCall) else step
            if step_name in self.symbol_table:
                effect = self.symbol_table[step_name].get('effect')
                if effect == 'Collapse_Wavefunction': print(f"      [WARNING] Step '{step_name}' causes '{effect}'.")
