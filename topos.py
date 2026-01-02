#!/usr/bin/env python3
import sys
import os
import argparse
from src.compiler import Lexer, Parser, SemanticAnalyzer, Interpreter, SemanticError

def main():
    parser = argparse.ArgumentParser(description="TOPOS-Xi Command Line Interface")
    parser.add_argument("command", choices=["check", "run"], help="Command to execute")
    parser.add_argument("file", help="Path to the .topology file")
    
    args = parser.parse_args()
    
    try:
        with open(args.file, 'r') as f:
            code = f.read()
    except FileNotFoundError:
        print(f"Error: File '{args.file}' not found.")
        sys.exit(1)

    print(f"--- Processing {args.file} ---")
    
    # 1. Lexing
    lexer = Lexer(code)
    try:
        tokens = lexer.tokenize()
    except SyntaxError as e:
        print(f"Syntax Error during Lexing: {e}")
        sys.exit(1)

    # 2. Parsing
    parser_obj = Parser(tokens)
    try:
        ast = parser_obj.parse_program()
    except SyntaxError as e:
        print(f"Syntax Error during Parsing: {e}")
        sys.exit(1)

    # 3. Semantic Analysis
    stdlib_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'src', 'stdlib')
    analyzer = SemanticAnalyzer(stdlib_path=stdlib_path)
    try:
        analyzer.visit_list(ast)
        print("\n[SUCCESS] Semantic Analysis Complete.")
    except SemanticError as e:
        print(f"\n[FAILURE] Semantic Error: {e}")
        sys.exit(1)

    # 4. Run (if requested)
    if args.command == "run":
        interpreter = Interpreter(analyzer.symbol_table)
        interpreter.run(ast)

if __name__ == "__main__":
    main()
