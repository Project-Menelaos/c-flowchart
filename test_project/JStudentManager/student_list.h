//
//  student_list.h
//  JStudentManager
//
//  Created by James Swineson on 1/2/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#ifndef student_list_h
#define student_list_h

#include "list.h"

typedef list student_list;

void student_list_data_init(list);
void student_list_node_data_init(list_node);
void student_list_node_data_free(list_node);
string student_list_node_data_serialize(list_node);
string student_list_data_serialize(list);

#endif /* student_list_h */
