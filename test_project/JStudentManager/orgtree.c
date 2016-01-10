//
//  orgtree.c
//  JStudentManager
//
//  Created by James Swineson on 1/5/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#include "orgtree.h"
#include "common.h"
#include "studentdb.h"
#include "list.h"
#include "model.h"
#include "organization_list.h"

/*

 > tree -F --comments
 ...
 |-- kernels/             Precompiled Linux 2.6.37.6 kernel images.
 |   |
 |   |-- hugesmp.s        The default standard install kernel for Slackware.
 |   |                    This supports pretty much everything in the
 |   |                    2.6.37.6 kernel, and includes support for Speakup.
 |   |                    This kernel requires at least a Pentium-Pro processor.
 |   |
 |   `-- huge.s           A single-processor version of huge.s that will
 |                        function with older hardware such as a 486 with
 |                        128MB (64MB _might_ work) or more of RAM.
 |                        This kernel also supports Speakup.
 ...

*/
static bool first_row = true;
static int indent_level = -1;
#define SQLCOUNTSTMT "SELECT COUNT(*) FROM `organizations` WHERE `parent` = %s ORDER BY id ASC"
#define SQLSTMT "SELECT * FROM `organizations` WHERE `parent` = %s ORDER BY id ASC"

int org_db_plain_display_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names) {
    int *p_rn = (int*)p_data;
    (*p_rn)++;
    printf("[%s]%s", p_fields[0], p_fields[1]);
    printf("\n");
    return 0;
}

int org_db_display_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names) {
    int *p_rn = (int*)p_data;
    (*p_rn)++;
    if (first_row) {
        first_row = false;
    }
    if (indent_level >= 0) {
        //printf("|");
        for (int i = 0; i < indent_level; i++) {
            printf("|   ");
        }
        printf("|-- ");
    }
    printf("[%s]%s", p_fields[0], p_fields[1]);
    printf("\n");
    indent_level++;
    {
        //int total_counts;
        bstring str = bformat(SQLSTMT, p_fields[0]);
        string s = bstr2cstr(str, ' ');
        sqlite_select_stmt_with_custom_callback(db, s, org_db_display_callback);
    }
    indent_level--;
    return 0;
}

void org_db_display()
{
    first_row = true;
    indent_level = -1;
    sqlite_select_stmt_with_custom_callback(db, "select * from `organizations` WHERE `parent` IS NULL ORDER BY id ASC", org_db_display_callback);
}

void org_db_add(long id, string name, long parent)
{
    if (id == -1) {
        sqlite_select_stmt(db, dsprintf("INSERT INTO `organizations` (name, parent) VALUES ('%s', '%ld')" , name, parent));
    } else {
        sqlite_select_stmt(db, dsprintf("INSERT INTO `organizations` (id, name, parent) VALUES ('%ld', '%s', '%ld')" , id, name, parent));
    }
}

int org_db_del(long id)
{
    return sqlite_sql_stmt(db, dsprintf("DELETE FROM `organizations` WHERE id=%ld", id));
}

void org_db_search(string s)
{
    char d[80];
    char r[80];
    sscanf(s, "%80s %[^\n]80s", d, r);
    sqlite_select_stmt_with_custom_callback(db, dsprintf("SELECT * FROM `organizations` WHERE `%s` LIKE '%%%s%%' ORDER BY id ASC", d, r), org_db_plain_display_callback);
}