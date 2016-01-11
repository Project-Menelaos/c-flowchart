#!/usr/bin/env python3

import sys, os, ntpath
from pycparser.pycparser import parse_file, c_parser, mermaid_generator

def node_type(node):
    if is_if_branch(node) or is_root(node):
        return 'none' # node_str(node)
    else:
        return node.content.strip().split('_')[1]

def node_id(node):
    return node.content.split('[')[0].split('(')[0].split('{')[0]

def node_str(node):
    return node.content[len(node_id(node))+2:-3].strip()

def is_if_true_branch(node):
    return (node_str(node) == "If-True")

def is_if_false_branch(node):
    return (node_str(node) == "If-False")

def is_if_branch(node):
    return (is_if_false_branch(node) or is_if_true_branch(node))

def is_root(node):
    return (node.content == "root")

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

def link(node1, node2, cond="", filter_loop=True):
    if filter_loop and (node1 == node2 or node_id(node1) == node_id(node2)):
        return
    if cond == "":
        print("%s --> %s" % (node_id(node1), node_id(node2)))
    else:
        print("%s -- %s --> %s"% (node_id(node1), cond, node_id(node2)))

def print_nodes(tree):
    s = str(tree.content).strip()
    if not is_if_branch(tree) and not is_root(tree):
        print(s)
    for i in tree.children:
        print_nodes(i)

def print_links(tree, last_node):
    if node_type(tree) == "FuncDef" and len(tree.children) > 0:
        print("%% FuncDef start")
        #print("subgraph " + node_id(tree))
        link(tree, tree.children[0])
        for i in tree.children:
            print_links(i, tree)
        #print("end")
    # print(r"%%Type: " + node_type(tree) + " branch?" + str(is_if_branch(tree)) + " root?" + str(is_root(tree)))
    # else:
    if node_type(tree) == "If":
        print("%% If start")
        if (len(tree.children) > 0):
            link(tree, tree.children[0].children[0], cond="True")
            for i in tree.children[0].children:
                print_links(i, tree)

            endnode = tree.children[0]
            while(len(endnode.children) > 0):
                endnode = endnode.children[-1]
            tree.if_end = [endnode]

        if (len(tree.children) > 1):
            link(tree, tree.children[1].children[0], cond="False")
            for i in tree.children[1].children:
                print_links(i, tree)

            endnode = tree.children[1]
            while(len(endnode.children) > 0):
                endnode = endnode.children[-1]
            tree.if_end.append(endnode)

    else:
        if node_type(last_node) == "If" and hasattr(last_node, 'if_end'):
            for i in last_node.if_end:
                link(i, tree)
            print("%% If end")
        if len(tree.children) == 0:
            pass
        elif len(tree.children) == 1:
            link(tree, tree.children[0])
        else:
            for i in range(1, len(tree.children)):
                if not is_if_branch(tree) and not is_root(tree):
                    link(tree.children[i - 1], tree.children[i])
        for i in tree.children:
            print_links(i, tree)
        pass

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
