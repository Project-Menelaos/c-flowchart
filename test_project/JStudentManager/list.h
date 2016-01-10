//
//  immutable_list.h
//  JStudentManager
//
//  Created by James Swineson on 1/2/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#ifndef immutable_list_h
#define immutable_list_h

#include "common.h"
#include "templates.h"

#define LIST_ACTION_COUNT 5
enum LIST_ACTION_TYPE {
    ACTION_INIT,
    ACTION_NODE_INIT,
    ACTION_NODE_FREE,
    ACTION_NODE_DATA_SERIALIZE,
    ACTION_DATA_SERIALIZE
};

#define list_action(Y) ((list_##Y##_function_type)(this)->actionset[Y])

typedef struct list_node {
    struct list_node *next_node;
    void *data;
} *list_node;

typedef struct list {
    long length;
    size_t data_size;
    void *actionset[LIST_ACTION_COUNT];
    list_node first_node;
} *list;

typedef list null_list;

typedef void(*list_type_specific_initializer_type)(list);
typedef bool(*list_type_specific_search_compare_function_type)(list, void*);
typedef void(*list_type_specific_enumerator_yield_callback_function_type)(list_node);
typedef int(*list_type_specific_sort_compare_function_type)(list_node, list_node);
typedef string(*list_type_specific_node_serializer_type)(list_node);
typedef void(*list_ACTION_INIT_function_type)(list);
typedef void(*list_ACTION_NODE_INIT_function_type)(list_node);
typedef void(*list_ACTION_NODE_FREE_function_type)(list_node);
typedef string(*list_ACTION_NODE_DATA_SERIALIZE_function_type)(list_node);
typedef string(*list_ACTION_DATA_SERIALIZE_function_type)(list);

list list_new(list*, list_type_specific_initializer_type);
void list_delete(list);
list list_append(list, list_node);
list list_remove(list, list_node);
list list_search(list, list_type_specific_search_compare_function_type, void*);
void list_foreach(list, list_type_specific_enumerator_yield_callback_function_type);
void list_qsort(list, list_type_specific_sort_compare_function_type);
string list_serialize(list);

void null_list_data_init(list);
void null_list_node_data_init(list_node);
void null_list_node_data_free(list_node);
string null_list_node_data_serialize(list_node);
string null_list_data_serialize(list);

#endif /* immutable_list_h */
