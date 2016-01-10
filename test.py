#!/usr/bin/env python3

import sys, os

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..'])

from pycparser.pycparser import parse_file, c_parser, c_generator


def translate_to_c(filename):
    """ Simply use the c_generator module to emit a parsed AST.
    """
    ast = parse_file(filename
            , clean_code=True, clean_comments=True, clean_macros=False
            , use_cpp=True
            , cpp_path='clang'
            , cpp_args=['-E', '-Wno-macro-redefined', r'-I/Users/james/Copy/Code/Menelaos/menelaos-test-project/testcode3/JStudentManager/JStudentManager', '-I'+os.path.dirname(os.path.abspath(__file__))+r'/pycparser/utils/fake_libc_include', '-nostdinc']
            )

    ast.show()
    #generator = c_generator.CGenerator()
    #print(generator.visit(ast))

#------------------------------------------------------------------------------
if __name__ == "__main__":
    #_zz_test_translate()
    if len(sys.argv) > 1:
        translate_to_c(sys.argv[1])
    else:
        print("Please provide a filename as argument")
