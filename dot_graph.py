#!/usr/bin/env python3

import sys, os, ntpath
from pycparser.pycparser import parse_file, c_parser, mermaid_generator

have_end_types = ("If", "FuncDef", "While", "For", "DoWhile_Do")
have_multi_cond_types = ("If")
multi_cond_subtypes = ("If-True", "If-False")
connect_to_start_types = ("Continue")
connect_to_end_types = ("Break")
force_function_end_types = ("Return")

current_func = None
call_stack = []

def call_stack_append(node):
    if len(call_stack) == 0 or node not in call_stack:
        call_stack.append(node)

def call_stack_pop(node=None):
    if node == None:
        return call_stack.pop()
    else:
        if node not in call_stack:
            return
        else:
            while(call_stack[-1] != node):
                call_stack.pop()
            return call_stack.pop()

def generate_end_id(node):
    if node == None: return
    if node.end_id != None:
        return node.end_id
    if node.type == "If":
        node.end_id = node_id(node).replace("If", "EndIf")
    if node.type == "FuncDef":
        node.end_id = node_id(node).replace("FuncDef", "EndFuncDef")
    if node.type == "While":
        node.end_id = node_id(node).replace("While", "EndWhile")
    if node.type == "For":
        node.end_id = node_id(node).replace("For", "EndFor")
    if node.type == "DoWhile_Do":
        node.end_id = node_id(node.children[-1])
    return node.end_id

def generate_end_node_content(node):
    if node == None: return
    if node.end_content != None:
        return node.end_content
    if node.type == "If":
        node.end_content = print('%s [shape=%s,label="%s"];' % (generate_end_id(node), get_shape(tree), "End If")) # generate_end_id(node) + node_surround(node)[0] + "\"" + "End If" + "\"" + node_surround(node)[1]
    if node.type == "FuncDef":
        node.end_content = print('%s [shape=%s,label="%s"];' % (generate_end_id(node), get_shape(tree), "End Function")) #generate_end_id(node) + node_surround(node)[0] + "\"" + "End Function" + "\"" + node_surround(node)[1]
    if node.type == "While":
        node.end_content = print('%s [shape=%s,label="%s"];' % (generate_end_id(node), get_shape(tree), "End While")) #generate_end_id(node) + node_surround(node)[0] + "\"" + "End While" + "\"" + node_surround(node)[1]
    if node.type == "For":
        node.end_content = print('%s [shape=%s,label="%s"];' % (generate_end_id(node), get_shape(tree), "End For"))#generate_end_id(node) + node_surround(node)[0] + "\"" + "End For" + "\"" + node_surround(node)[1]
    if node.type == "DoWhile_Do":
        node.end_content = node.children[-1].content
    return node.end_content

def node_type(node):
    if node == None: return ""
    if is_if_branch(node) or is_root(node):
        return node.type
    elif type(node) is str:
        string = node
    else:
        string = node.content

    a = string.strip().split('_')
    try:
        int(a[2])
        return a[1]
    except:
        return '_'.join(a[1:2])

def node_id(node):
    if node == None: return ""
    if type(node) is str:
        string = node
    else:
        string = node.content
    return string.split('[')[0].split('(')[0].split('{')[0]


def node_surround(node):
    try:
        return node.surround
    except:
        return None

def node_str(node):
    try:
        return node.content[len(node_id(node))+2:-3].strip()
    except:
        return node

def is_if_true_branch(node):
    try:
        return (node.type == "If-True")
    except:
        return False

def is_if_false_branch(node):
    try:
        return (node.type == "If-False")
    except:
        return False

def is_if_branch(node):
    return (is_if_false_branch(node) or is_if_true_branch(node))

def is_root(node):
    try:
        return (node.type == "root")
    except:
        return False

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
        if node_type(node1) in multi_cond_subtypes: return
        node_1_id = node_id(node1)
    if type(node2) is str:
        node_2_id = node2
    else:
        if node_type(node2) in multi_cond_subtypes: return
        node_2_id = node_id(node2)
    if filter_loop and (node_1_id == node_2_id):
        return
    if not cond:
        print("%s -> %s;" % (node_1_id, node_2_id))
    else:
        print("%s:%s -> %s"% (node_1_id, "s0" if cond=="True" else "s1", node_2_id))

def get_shape(node):
    if node.surround == "[]": return "record"
    if node.surround == "{}": return "diamond"
    if node.surround == "()": return "ellipse"

def print_nodes(tree):
    # print("%% [Node]: " + node_id(tree) + " Type: " + node_type(tree) + " Str: " + node_str(tree))
    s = str(tree.content).strip()
    if not is_if_branch(tree) and not is_root(tree):
        # print(s)
        print('%s [shape=%s,label="%s"]' % (node_id(tree), get_shape(tree), node_str(tree)))
    if tree.type in have_end_types:
        print(generate_end_node_content(tree))
    for i in tree.children:
        print_nodes(i)

def iterative_print_links(tree, last_node, cond=None, function_end=None):
    global call_stack
    if len(tree.children) > 0:
        # print("%% Iterate over normal stmt")
        # (before) --> (current tree)
        if node_type(tree) in multi_cond_subtypes:
            link(last_node, tree.children[0], cond=cond)
        elif node_type(last_node) in have_end_types:
            link(tree.children[0], generate_end_id(last_node), cond=cond)
        else:
            link(tree, tree.children[0], cond=cond)

        # (current tree) --> (current tree)
        for i in range(1, len(tree.children)):
            if (tree.children[i - 1].type not in have_end_types):
                link(tree.children[i - 1], tree.children[i])
            else:
                link(generate_end_id(tree.children[i - 1]), tree.children[i])

        # inside every node
        for i in tree.children:
            print_links(i, tree, function_end=function_end)

        # print_links(tree.children[0], tree, call_stack=call_stack, function_end=function_end)
        # for i in range(1, len(tree.children)):
        #     print_links(tree.children[i], tree.children[i - 1], call_stack=call_stack, function_end=function_end)
    else:
        if node_type(last_node) not in have_end_types:
            link(tree, last_node)


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
    # print("%% Last call: " + node_str(last_call))
    return last_call

def print_links(tree, last_node, function_end=None):
    # print("%% Got node type: "+str(node_type(tree)))
    global current_func
    global call_stack

    # for i in call_stack:
    #     print("%% [Call Stack] " + node_str(i))

    if node_type(tree) == "root":
        for i in tree.children:
            print_links(i, tree)
        return

    elif node_type(tree) == "FuncDef" and len(tree.children) > 0:
        print("subgraph g_%s {" % node_id(tree))
        print('label = "%s";' % node_str(tree))
        call_stack_append(tree)
        current_func = tree
        iterative_print_links(tree, last_node, function_end=tree)
        func_last_call_node = search_last_call(tree)
        if node_type(func_last_call_node) not in force_function_end_types:
            link(func_last_call_node, generate_end_id(tree))
        print("}")
        call_stack_pop(tree)
        current_func = None
        return

    elif node_type(tree) == "If":
        call_stack_append(tree)
        if_end_node_id = generate_end_id(tree)
        # print('%% If node: ' + node_id(tree))
        # If-True
        # print('%% If-True')
        # link(tree, tree.children[0].children[0], "Tree")
        iterative_print_links(tree.children[0], tree, "True")
        if_true_end_node = search_last_call(tree.children[0])
        # print('%% If-True->end: ' + node_id(tree) + node_id(if_true_end_node))
        link(if_true_end_node, if_end_node_id)

        # If-False
        # print('%% If-False')
        if len(tree.children) > 1:  # we got a else
            iterative_print_links(tree.children[1], tree, "False")
            if_false_end_node = search_last_call(tree.children[1])
            # print('%% If-False->end: ' + node_id(tree) + node_id(if_false_end_node))
            link(if_false_end_node, if_end_node_id)
        else:   # no else
            link(tree, if_end_node_id, "False")

        call_stack_pop(tree)
        # for i in range(2, len(tree.children)):
        #     iterative_print_links(tree.children[i], tree, call_stack=call_stack)
        return

    elif node_type(tree) == "While":
        call_stack_append(tree)
        # print("%% While start")
        iterative_print_links(tree, last_node)
        # link(search_last_call(tree), generate_end_id(tree))
        # print("%% While end")
        call_stack_pop(tree)
        link(search_last_call(tree), generate_end_id(tree))
        return

    elif node_type(tree) == "For":
        call_stack_append(tree)
        # print("%% For start")
        iterative_print_links(tree, last_node)
        link(search_last_call(tree), generate_end_id(tree))
        link(generate_end_id(tree), tree)
        # print("%% For end")
        call_stack_pop(tree)
        return

    elif node_type(tree) == "DoWhile_Do":
        call_stack_append(tree)
        # print("%% DoWhile start")
        iterative_print_links(tree, last_node)
        link(generate_end_id(tree), tree)
        # print("%% DoWhile end")
        call_stack_pop(tree)
        return

    elif node_type(tree) in connect_to_start_types:
        link(tree, call_stack[-1])
        return

    elif node_type(tree) in connect_to_end_types:
        link(tree, generate_end_id(call_stack[-1]))
        return


    elif node_type(tree) in force_function_end_types:
        link(tree, generate_end_id(current_func))
        return

    # normal
    call_stack_append(tree)
    iterative_print_links(tree, last_node)
    call_stack_pop(tree)


def print_mermaid_graph(tree, file):
    # print("graph TB")
    # print(r"%% " + "Nodes")
    print('digraph "%s" {' % file)
    print('label="Flowchart of %s";' % file)
    print_nodes(tree)
    # print(r"%% " + "Links")
    print_links(tree, tree)
    print('}')

if __name__ == "__main__":
    if len(sys.argv) > 1:
        # print(r"%% " + ntpath.basename(sys.argv[1]))
        # print(r"%% " + "Generated by c-flowchart")
        tree = generate_call_tree(sys.argv[1])
        print_mermaid_graph(tree, sys.argv[1])
    else:
        print("Please provide a filename as argument")
