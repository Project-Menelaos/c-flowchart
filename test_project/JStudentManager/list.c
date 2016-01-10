//
//  immutable_list.c
//  JStudentManager
//
//  Created by James Swineson on 1/2/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#include "list.h"

void null_list_data_init(list this)
{
    this->data_size = 0;
    this->actionset[ACTION_INIT] = null_list_data_init;
    this->actionset[ACTION_NODE_INIT] = null_list_node_data_init;
    this->actionset[ACTION_NODE_FREE] = null_list_node_data_free;
    this->actionset[ACTION_NODE_DATA_SERIALIZE] = null_list_node_data_serialize;
    this->actionset[ACTION_DATA_SERIALIZE] = null_list_data_serialize;
};


void null_list_node_data_init(list_node this)
{
    this->data = NULL;
};

void null_list_node_data_free(list_node this)
{
    // nothing to do
};

string null_list_node_data_serialize(list_node this)
{
    return "NULL node";
};

string null_list_data_serialize(list this)
{
    string s = null_list_node_data_serialize(this->first_node);
    free(s);
    return "NULL list";
}

list_node list_node_new(list this)
{
    list_node new_node = malloc(sizeof(list_node));
    new_node->next_node = NULL;
    list_action(ACTION_NODE_INIT)(new_node);
    return new_node;
}

void list_node_free(list this, list_node node)
{
    if (!node) return;
    list_action(ACTION_NODE_FREE)(node);
    if (node->data) free(node->data);
    free(node);
}

list list_new(list *this, list_type_specific_initializer_type init)
{
    *this = (list)malloc(sizeof(struct list));
    (*this)->length = 0;
    (*this)->first_node = NULL;
    init(*this);
    return *this;
};

void list_delete(list this)
{
    list_node temp;
    while (this->length) {
        temp = this->first_node->next_node;
        list_node_free(this, this->first_node);
        this->first_node = temp;
        this->length--;
    }
    free(this);
};

list list_append(list this, list_node new_node)
{
    if (this->length)
    {
        list_node last = this->first_node;
        while (last->next_node) last = last->next_node;
        last->next_node = new_node;
    } else {
        this->first_node = new_node;
    }
    this->length++;
    return this;
};

list list_remove(list this, list_node node)
{
    if (!(this->first_node == node)) {
        // not the first one
        list_node before = this->first_node;
        while (before->next_node) {
            if (before->next_node == node) {
                before->next_node = node->next_node;
                break;
            }
        }
    } else {
        this->first_node = node->next_node;
    }
    list_node_free(this, node);
    this->length--;
    return this;
};

list list_search(list, list_type_specific_search_compare_function_type, void*);

void list_foreach(list this, list_type_specific_enumerator_yield_callback_function_type yield_func)
{
    list_node current_node = this->first_node;
    while (current_node) {
        yield_func(current_node);
        current_node = current_node -> next_node;
    }
}

void list_qsort(list, list_type_specific_sort_compare_function_type);

string list_serialize(list this)
{
    return list_action(ACTION_DATA_SERIALIZE)(this);
}