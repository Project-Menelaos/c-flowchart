#!/usr/bin/env python3

import sys, os
from pycparser.pycparser import parse_file, c_parser, mermaid_generator

def print_call_tree(tree, level=0):
    if (level > 0):
        print("|   " * (level - 1), end="")
        print("`-- ", end="")
    print(str(tree.content).strip())
    for i in tree.children:
        print_call_tree(i, level + 1)

def display_call_tree(filename):
    """ Simply use the c_generator module to emit a parsed AST.
    """
    ast = parse_file(filename
            , clean_code=True, clean_comments=True, clean_macros=False
            , use_cpp=True
            , cpp_path='clang'
            , cpp_args=['-E', '-Wno-macro-redefined', '-I'+os.path.dirname(os.path.abspath(__file__))+r'/pycparser/utils/fake_libc_include', '-nostdinc']
            )

    #ast.show()
    generator = mermaid_generator.MermaidGenerator()
    tree = generator.get_call_tree(ast)
    print_call_tree(tree)

#------------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) > 1:
        display_call_tree(sys.argv[1])
    else:
        print("Please provide a filename as argument")
