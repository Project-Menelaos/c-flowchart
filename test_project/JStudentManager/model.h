//
//  db.h
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#ifndef model_h
#define model_h

#include "common.h"
#include "sqlite3.h"

typedef sqlite3 database;

int sqlite_select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names);
void sqlite_select_stmt(database *db, const char* stmt);
void sqlite_select_stmt_with_custom_callback(database *db, const char* stmt, int (*sqlite_select_callback)(void *p_data, int num_fields, char **p_fields, char **p_col_names));
int sqlite_sql_stmt(database *db, const char* stmt);

#endif /* db_h */
