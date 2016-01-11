#!/usr/bin/env python3

import sys, os, ntpath
from pycparser.pycparser import parse_file, c_parser, mermaid_generator

have_end_types = ("If", "FuncDef", "while")
have_multi_cond_types = ("If")
multi_cond_subtypes = ("If-True", "If-False")

def generate_end_id(node):
    if node.type == "If":
        return node_id(node).replace("If", "EndIf")
    if node.type == "FuncDef":
        return node_id(node).replace("FuncDef", "EndFuncDef")
    if node.type == "While":
        return node_id(node).replace("While", "EndWhile")

def generate_end_node_content(node):
    if node.type == "If":
        return generate_end_id(node) + node_surround(node)[0] + "\"" + "End If" + "\"" + node_surround(node)[1]
    if node.type == "FuncDef":
        return generate_end_id(node) + node_surround(node)[0] + "\"" + "End Function" + "\"" + node_surround(node)[1]
    if node.type == "While":
        return generate_end_id(node) + node_surround(node)[0] + "\"" + "End While" + "\"" + node_surround(node)[1]

connect_to_start_types = ("continue")
connect_to_end_types = ("break")
force_function_end_types = ("return")

def node_type(node):
    if is_if_branch(node) or is_root(node):
        return node.type
    else:
        return node.content.strip().split('_')[1]

def node_id(node):
    return node.content.split('[')[0].split('(')[0].split('{')[0]

def node_surround(node):
    try:
        return node.surround
    except:
        return None

def node_str(node):
    return node.content[len(node_id(node))+2:-3].strip()

def is_if_true_branch(node):
    return (node.type == "If-True")

def is_if_false_branch(node):
    return (node.type == "If-False")

def is_if_branch(node):
    return (is_if_false_branch(node) or is_if_true_branch(node))

def is_root(node):
    return (node.type == "root")

def generate_call_tree(filename):
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
    return tree

def link(node1, node2, cond=None, filter_loop=True):
    if type(node1) is str:
        node_1_id = node1
    else:
        node_1_id = node_id(node1)
    if type(node2) is str:
        node_2_id = node2
    else:
        node_2_id = node_id(node2)
    if filter_loop and (node_1_id == node_2_id):
        return
    if not cond:
        print("%s --> %s" % (node_1_id, node_2_id))
    else:
        print("%s -- %s --> %s"% (node_1_id, cond, node_2_id))

def print_nodes(tree):
    s = str(tree.content).strip()
    if not is_if_branch(tree) and not is_root(tree):
        print(s)
    if tree.type in have_end_types:
        print(generate_end_node_content(tree))
    for i in tree.children:
        print_nodes(i)

def iterative_print_links(tree, last_node, cond=None, call_stack=[], function_end=None):
    if len(tree.children) > 0:
        # print("%% Iterate over normal stmt")
        if node_type(tree) in multi_cond_subtypes:
            link(last_node, tree.children[0], cond=cond)
        else:
            link(tree, tree.children[0], cond=cond)
        for i in range(1, len(tree.children)):
            if tree.children[i - 1].type not in have_end_types:
                link(tree.children[i - 1], tree.children[i])
            else:
                link(generate_end_id(tree.children[i - 1]), tree.children[i])
        for i in tree.children:
            print_links(i, tree, call_stack=call_stack, function_end=function_end)

def search_last_call(node):
    last_call = node
    # if last_call.type in have_end_types:
    #     return generate_end_id(last_call)
    while len(last_call.children) > 0:
        if last_call.children[-1].type in have_end_types:
            last_call = generate_end_id(last_call.children[-1])
            break
        else:
            last_call = last_call.children[-1]
    return last_call

def print_links(tree, last_node, call_stack=[], function_end=None):
    if node_type(tree) in have_end_types:
        call_stack.append(tree)

    if node_type(tree) == "root":
        for i in tree.children:
            print_links(i, tree)
        return

    elif node_type(tree) == "FuncDef" and len(tree.children) > 0:
        print("%% FuncDef start")
        iterative_print_links(tree, last_node, call_stack=call_stack, function_end=tree)
        func_last_call_node = search_last_call(tree)
        link(func_last_call_node, generate_end_id(tree))
        print("%% FuncDef end")
        return

    elif node_type(tree) == "If":
        # tree.if_end = []
        if_end_node_id = generate_end_id(tree)
        # If-True
        print('%% If-True')
        iterative_print_links(tree.children[0], tree, "True")
        if_true_end_node = search_last_call(tree.children[0])
        link(if_true_end_node, if_end_node_id)

        # If-False
        print('%% If-False')
        if len(tree.children) > 1:  # we got a else
            iterative_print_links(tree.children[1], tree, "False")
            if_false_end_node = search_last_call(tree.children[1])
            link(if_false_end_node, if_end_node_id)
        else:   # no else
            link(tree, if_end_node_id, "False")

        return

    elif node_type(tree) == "While":
        print("%% While start")
        iterative_print_links(tree, last_node, call_stack=call_stack)
        while_last_call_node = search_last_call(tree)
        link(while_last_call_node, generate_end_id(tree))
        print("%% While end")
        return

    # normal
    iterative_print_links(tree, last_node, call_stack=call_stack)


def print_mermaid_graph(tree):
    print("graph TB")
    print(r"%% " + "Nodes")
    print_nodes(tree)
    print(r"%% " + "Links")
    print_links(tree, tree)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        print(r"%% " + ntpath.basename(sys.argv[1]))
        print(r"%% " + "Generated by c-flowchart")
        tree = generate_call_tree(sys.argv[1])
        print_mermaid_graph(tree)
    else:
        print("Please provide a filename as argument")
