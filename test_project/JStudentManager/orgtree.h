//
//  orgtree.h
//  JStudentManager
//
//  Created by James Swineson on 1/5/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#ifndef orgtree_h
#define orgtree_h

#include "common.h"

void org_db_display();
void org_db_add(long id, string name, long parent);
int org_db_del(long id);
void org_db_search(string s);

#endif /* orgtree_h */
