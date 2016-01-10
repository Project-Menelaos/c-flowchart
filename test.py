#!/usr/bin/env python3

import sys, os

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..'])

from pycparser.pycparser import parse_file, c_parser, mermaid_generator

def print_list_tree(lst, level=0):
    for l in lst[0:]:
        if type(l) is list:
            print_list_tree(l, level + 1)
        else:
            if (level >= 0):
                print("|   " * level, end="")
                print("|-- ", end="")
            print(str(l).strip())

def translate_to_c(filename):
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
    generator.visit(ast)
    #print_list_tree(generator.visit(ast))
    a = generator.visit_stack
    print_list_tree(a)

#------------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) > 1:
        translate_to_c(sys.argv[1])
    else:
        print("Please provide a filename as argument")
