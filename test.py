#!/usr/bin/env python3

import sys

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..'])

from pycparser import parse_file, c_parser, c_generator


def translate_to_c(filename):
    """ Simply use the c_generator module to emit a parsed AST.
    """
    ast = parse_file(filename
            , use_cpp=True
            #, cpp_path='clang'
            , cpp_args=['-E', '-Wno-macro-redefined', r'-I/Users/james/Copy/Code/Menelaos/menelaos-test-project/testcode3/JStudentManager/JStudentManager', r'-I/Users/james/Copy/Code/Menelaos/c-flowchart/pycparser-master/utils/fake_libc_include', '-nostdinc']
            )

    ast.show()
    generator = c_generator.CGenerator()
    print(generator.visit(ast))


def _zz_test_translate():
    # internal use
    src = r'''
    void f(char * restrict joe){}
int main(void)
{
    unsigned int long k = 4;
    int p = - - k;
    return 0;
}
'''
    parser = c_parser.CParser()
    ast = parser.parse(src)
    ast.show()
    generator = c_generator.CGenerator()

    print(generator.visit(ast))

    # tracing the generator for debugging
    #~ import trace
    #~ tr = trace.Trace(countcallers=1)
    #~ tr.runfunc(generator.visit, ast)
    #~ tr.results().write_results()


#------------------------------------------------------------------------------
if __name__ == "__main__":
    #_zz_test_translate()
    if len(sys.argv) > 1:
        translate_to_c(sys.argv[1])
    else:
        print("Please provide a filename as argument")
