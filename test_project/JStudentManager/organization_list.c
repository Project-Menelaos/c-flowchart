//
//  organization_list.c
//  JStudentManager
//
//  Created by James Swineson on 1/2/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#include "organization_list.h"

org_list organizations;

void org_list_data_init(list this)
{
    this->data_size = sizeof(struct org_list_data);
    this->actionset[ACTION_INIT] = org_list_data_init;
    this->actionset[ACTION_NODE_INIT] = org_list_node_data_init;
    this->actionset[ACTION_NODE_FREE] = org_list_node_data_free;
    this->actionset[ACTION_NODE_DATA_SERIALIZE] = org_list_node_data_serialize;
    this->actionset[ACTION_DATA_SERIALIZE] = org_list_data_serialize;
};

void org_list_node_data_init(list_node this)
{
    //if (this->data != NULL) throw(ArgumentException, "Node is not empty");
    this->data = (org_list_data)malloc(sizeof(struct org_list_data));
};

void org_list_node_data_free(list_node this)
{
    free(this->data);
    this->data = NULL;
};

string org_list_node_data_serialize(list_node this)
{
    // TODO
    org_list_data data = this->data;
    bstring str = bformat("Organization: #%ld, %s, parent #%ld\n", data->id, data->name, data->parent);
    string s = bstr2cstr(str, ' ');
    string result = malloc(strlen(s) * sizeof(char));
    strcpy(result, s);
    return result;
};

string org_list_data_serialize(list this)
{
    // TODO
    return "org list";
}
