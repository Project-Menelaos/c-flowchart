//
//  stulist.c
//  JStudentManager
//
//  Created by James Swineson on 1/5/16.
//  Copyright © 2016 James Swineson. All rights reserved.
//

#include "stulist.h"
#include "common.h"
#include "studentdb.h"
#include "list.h"
#include "model.h"

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

#define SQLSTUSTMT "SELECT id, name, sex, age FROM `students` WHERE `organization` = %ld ORDER BY id ASC"
#define SQLORGSTMT "SELECT * FROM `organizations` WHERE `parent` = %s ORDER BY id ASC"

int stu_db_display_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names) {
    int *p_rn = (int*)p_data;
    (*p_rn)++;
    for(int i = 0; i < num_fields; i++) {
        if (i == 2) printf("%-20s", p_fields[i][0]=='0'?"Male":"Female");
        else printf("%-20s", p_fields[i]);
    }
    printf("\n");
    //sqlite_select_stmt_with_custom_callback(db, , stu_db_display_callback);
    return 0;
}

void stu_db_display(long org)
{
    int num_fields = 4;
    string p_col_names[] = {
        "ID",
        "Name",
        "Sex",
        "Age",
    };
    for(int i = 0; i < num_fields; i++) {
        printf("%-20s", p_col_names[i]);
    }
    printf("\n");
    for(int i = 0; i< num_fields*20; i++) {
        printf("*");
    }
    printf("\n");
    sqlite_select_stmt_with_custom_callback(db, dsprintf(SQLSTUSTMT, org), stu_db_display_callback);
}

void stu_db_add(long id, string name, int sex, int age, long org)
{
    if (id == -1) {
        sqlite_select_stmt(db, dsprintf("INSERT INTO `students` (name, sex, age, organization) VALUES ('%s', %d, %d, %ld)", name, sex, age, org));
    } else {
        sqlite_select_stmt(db, dsprintf("INSERT INTO `students` (id, name, sex, age, organization) VALUES (%ld, '%s', %d, %d, %ld)", id, name, sex, age, org));
    }
}

int stu_db_del(long id)
{
    return sqlite_sql_stmt(db, dsprintf("DELETE FROM `students` WHERE id=%ld", id));
}

void stu_db_search(string s)
{
    int num_fields = 4;
    string p_col_names[] = {
        "ID",
        "Name",
        "Sex",
        "Age",
    };
    for(int i = 0; i < num_fields; i++) {
        printf("%-20s", p_col_names[i]);
    }
    printf("\n");
    for(int i = 0; i< num_fields*20; i++) {
        printf("*");
    }
    printf("\n");
    char d[80];
    char r[80];
    sscanf(s, "%80s %[^\n]80s", d, r);
    sqlite_select_stmt_with_custom_callback(db, dsprintf("SELECT * FROM `students` WHERE `%s` LIKE '%%%s%%' ORDER BY id ASC", d, r), stu_db_display_callback);
}