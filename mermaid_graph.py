#!/usr/bin/env python3

import sys, os, ntpath
from pycparser.pycparser import parse_file, c_parser, mermaid_generator

def node_type(node):
    if is_if_branch(node) or is_root(node):
        return node.type
    else:
        return node.content.strip().split('_')[1]

def node_id(node):
    return node.content.split('[')[0].split('(')[0].split('{')[0]

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
    if tree.type == "If":
        print(node_id(tree).replace("If", "EndIf") + "[End If]")
    for i in tree.children:
        print_nodes(i)

def generate_if_end_id(node):
    if node.type == "If":
        return node_id(node).replace("If", "EndIf")

def print_links(tree, last_node, force_connected_end_to=None):
    if node_type(tree) == "root":
        for i in tree.children:
            print_links(i, tree)
        return

    if node_type(tree) == "FuncDef" and len(tree.children) > 0:
        print("%% FuncDef start")
        link(tree, tree.children[0])
        # let them happen as normal!
        # for i in range(1, len(tree.children)):
        #     if tree.children[i - 1].type not in ("If"):
        #         link(tree.children[i - 1], tree.children[i])
        # for i in tree.children:
        #     print_links(i, tree)
        # print("%% FuncDef end")
        # return

    if node_type(tree) == "If":
        # tree.if_end = []
        if_end_node_id = generate_if_end_id(tree)
        # If-True
        print('%% If-True')
        link(tree, tree.children[0].children[0], "True")
        for i in tree.children[0].children:
            print_links(i, tree)
            # if a is mermaid_generator.MermaidGenerator.H:
            #     tree.if_end.append(a)
        if_true_end_node = tree.children[0]
        while len(if_true_end_node.children) > 0:
            if_true_end_node = if_true_end_node.children[-1]

        link(if_true_end_node, if_end_node_id)

        # If-False
        print('%% If-False')
        if len(tree.children) > 1:
            link(tree, tree.children[1].children[0], "False")
            for i in tree.children[1].children:
                print_links(i, tree)

            if_false_end_node = tree.children[0]
            while len(if_false_end_node.children) > 0:
                if_false_end_node = if_false_end_node.children[-1]
            link(if_false_end_node, if_end_node_id)

        return

    # normal
    if len(tree.children) > 0:
        print("%% Iterate over normal stmt")
        link(tree, tree.children[0])
        for i in range(1, len(tree.children)):
            if tree.children[i - 1].type not in ("If"):
                link(tree.children[i - 1], tree.children[i])
            else:
                link(generate_if_end_id(tree.children[i - 1]), tree.children[i])
        for i in tree.children:
            print_links(i, tree)


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
