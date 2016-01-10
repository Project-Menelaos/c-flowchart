//
//  stulist.h
//  JStudentManager
//
//  Created by James Swineson on 1/5/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#ifndef stulist_h
#define stulist_h

#include "common.h"

int stu_db_display_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names);
void stu_db_display();
void stu_db_add(long id, string name, int sex, int age, long org);
int stu_db_del(long id);
void stu_db_search(string s);

#endif /* stulist_h */
