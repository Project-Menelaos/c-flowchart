//
//  commands.c
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#include "common.h"
#include "commands.h"
#include "studentdb.h"
#include "orgtree.h"
#include "stulist.h"
#include "model.h"

COMMAND(sqlite_load)
{
    try {
        database_initialize();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(sqlite_save)
{
    try {
        database_close();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(sqlite_query_all_orgs)
{
    puts("DEBUG: printing all orgs from sqlite db");
    try {
        database_query_all_orgs();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(sqlite_query_all_students)
{
    puts("DEBUG: printing all students from sqlite db");
    try {
        database_query_all_students();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(sqlite_to_list)
{
    try {
        database_to_org_list();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(sqlite_from_list);

COMMAND(list_query_all_orgs)
{
    puts("DEBUG: printing all orgs from list db");
    try {
        list_print_all_orgs();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(display_orgtree)
{
    try {
        org_db_display();
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(sql)
{
    sqlite_select_stmt(db, args);
    return EXIT_SUCCESS;
}

COMMAND(org_add)
{
    if (args == NULL) throw(ArgumentNullException, "No arguments specified");
    long parent;
    char name[MAX_ORG_NAME_LEN + 1];
    
    if (sscanf(args, "%ld %[^\n]" TOSTRING(MAX_ORG_NAME_LEN) "s", &parent, name) != 2) throw(ArgumentException, "Unable to intercept arguments");
    org_db_add(-1, name, parent);
    return EXIT_SUCCESS;
}

COMMAND(org_del)
{
    // TODO: delete recursively
    if (args == NULL) throw(ArgumentNullException, "No arguments specified");
    while (isspace(*args)) args++;
    sqlite_select_stmt(db, dsprintf("DELETE FROM `organizations` WHERE id='%s' OR name='%s'" , args, args));
    return EXIT_SUCCESS;
}

COMMAND(org_modify)
{
    if (args == NULL) throw(ArgumentNullException, "No arguments specified");
    long id;
    long parent;
    char name[MAX_ORG_NAME_LEN + 1];
    if (sscanf(args, "%ld %ld %[^\n]" TOSTRING(MAX_ORG_NAME_LEN) "s", &id, &parent, name) != 3) throw(ArgumentException, "Unable to intercept arguments");
    transaction_begin();
    if (org_db_del(id) != DB_OK) {
        puts("Error: organization not found");
        transaction_rollback();
        return EXIT_SUCCESS;
    }
    org_db_add(id, name, parent);
    transaction_commit();
    return EXIT_SUCCESS;
}

COMMAND(org_search)
{
    if (args == NULL) throw(ArgumentNullException, "No arguments specified");
    while (isspace(*args)) {
        args++;
    }
    org_db_search(args);
    return EXIT_SUCCESS;
}

COMMAND(display_student)
{
    try {
        long org;
        sscanf(args, "%ld", &org);
        stu_db_display(org);
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(stu_add)
{
    long id;
    char name[MAX_STU_NAME_LEN];
    bool sex;
    int age;
    long org;
    int temp;
    clearbuffer(stdin);
    printf("ID[auto]: ");
    temp = getchar();
    if (temp != '\n') {
        char ids[100];
        ids[0] = temp;
        scanf("%s", ids + 1);
        sscanf(ids, "%ld", &id);
    } else id = -1;
    clearbuffer(stdin);
    printf("Name: ");
    scanf(STU_NAME_FMT, name);
    clearbuffer(stdin);
    printf("Sex[M/F]: ");
    temp = getchar();
    sex = (temp == 'M')?0:1; // TODO: check value
    clearbuffer(stdin);
    printf("Age: ");
    scanf("%d", &age);
    clearbuffer(stdin);
    printf("Organization: ");
    scanf("%ld", &org);
    clearbuffer(stdin);
    try {
        stu_db_add(id, name, sex, age, org);
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(stu_del)
{
    try {
        long id;
        sscanf(args, "%ld", &id);
        stu_db_del(id);
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    return EXIT_SUCCESS;
}

COMMAND(stu_modify)
{
    long id;
    char name[MAX_STU_NAME_LEN];
    bool sex;
    int age;
    long org;
    int temp;
    sscanf(args, "%ld", &id);
    transaction_begin();
    if (stu_db_del(id) != DB_OK) {
        puts("Error: student not found");
        transaction_rollback();
        return EXIT_SUCCESS;
    }
    clearbuffer(stdin);
    printf("Name: ");
    scanf(STU_NAME_FMT, name);
    clearbuffer(stdin);
    printf("Sex[M/F]: ");
    temp = getchar();
    sex = (temp == 'M')?0:1; // TODO: check value
    clearbuffer(stdin);
    printf("Age: ");
    scanf("%d", &age);
    clearbuffer(stdin);
    printf("Organization: ");
    scanf("%ld", &org);
    clearbuffer(stdin);
    try {
        stu_db_add(id, name, sex, age, org);
    } catch() {
        fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
    }
    transaction_commit();
    return EXIT_SUCCESS;
}

COMMAND(stu_search)
{
    while (isspace(*args)) {
        args++;
    }
    stu_db_search(args);
    return EXIT_SUCCESS;
}