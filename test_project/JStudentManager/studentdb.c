//
//  studentdb.c
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#include "studentdb.h"
#include "common.h"
#include "model.h"
#include "organization_list.h"

database* db;
const string db_path = "/Users/james/Copy/Code/Data Structure/JStudentManager/JStudentManager/studentdb";
static int first_row;

bool fileExists(const char *filename)
{
    FILE *fp = fopen (filename, "r");
    if (fp!=NULL) fclose (fp);
    return (fp!=NULL);
}

void database_initialize()
{
    if (!fileExists(db_path)) throw(FileNotFoundException, "Unable to open database file");
    sqlite3_open(db_path, &db);
    if(db == NULL) throw(FormatException, "Unable to parse database");
}

void database_close()
{
    if (db == NULL) throw(IOException, "Database is not opened");
    sqlite3_close(db);
    db = NULL;
}

void database_query_all_orgs()
{
    if (db == NULL) throw(DbException, "Database does not exist");
    sqlite_select_stmt(db, "select * from `organizations`");
}

void database_query_all_students()
{
    if (db == NULL) throw(DbException, "Database does not exist");
    sqlite_select_stmt(db, "select * from `students`");
}

int database_to_org_list_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names) {
    
    int i;
    int *p_rn = (int*)p_data;
    (*p_rn)++;
    
    list_node new_org_node = new(list_node);
    org_list_node_data_init(new_org_node);
    org_list_data data = (org_list_data)new_org_node->data;
    for(i=0; i < num_fields; i++) {
        //printf("%20s", p_fields[i]);
        //fflush(stdout);
        //continue;
        switch (i) {
            case 0:
                sscanf(p_fields[i], "%ld", &(data->id));
                break;
            case 1:
                data->name = malloc((strlen(p_fields[i]) + 1) * sizeof(char));
                sscanf(p_fields[i], "%s", data->name);
            case 2:
                if (p_fields[i] == NULL) {
                    data->parent = data->id;
                } else {
                    sscanf(p_fields[i], "%ld", &(data->parent));
                }
            default:
                break;
        }
    }
    list_append(organizations, new_org_node);
    //printf("\n");
    return 0;
}

void database_to_org_list()
{
    if (organizations != NULL) list_delete(organizations);
    list_new(&organizations, org_list_data_init);
    sqlite_select_stmt_with_custom_callback(db, "select * from `organizations`", database_to_org_list_callback);
}

void print_org_node(list_node this)
{
    string str = ((list_ACTION_NODE_DATA_SERIALIZE_function_type)(organizations->actionset[ACTION_NODE_DATA_SERIALIZE]))(this);
    printf("\t%s", str);
    free(str);
}

void list_print_all_orgs()
{
    list_foreach(organizations, print_org_node);
}

string build_org_list()
{
    // TODO
    return NULL;
}