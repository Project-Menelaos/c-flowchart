//
//  studentdb.h
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#pragma once
#ifndef studentdb_h
#define studentdb_h
#include "common.h"

extern database* db;
extern const string db_path;

void database_initialize();
void database_close();
void database_query_all_orgs();
void database_query_all_students();
void database_to_org_list();

void list_print_all_orgs();

#define transaction_begin() sqlite_sql_stmt(db, "begin");
#define transaction_commit() sqlite_sql_stmt(db, "commit");
#define transaction_rollback() sqlite_sql_stmt(db, "rollback");

#define DB_OK           0
#define DB_ERROR        1
#define DB_INTERNAL     2 
#define DB_PERM         3 
#define DB_ABORT        4 
#define DB_BUSY         5 
#define DB_LOCKED       6 
#define DB_NOMEM        7 
#define DB_READONLY     8 
#define DB_INTERRUPT    9 
#define DB_IOERR       10 
#define DB_CORRUPT     11 
#define DB_NOTFOUND    12 
#define DB_FULL        13
#define DB_CANTOPEN    14 
#define DB_PROTOCOL    15 
#define DB_EMPTY       16 
#define DB_SCHEMA      17 
#define DB_TOOBIG      18 
#define DB_CONSTRAINT  19 
#define DB_MISMATCH    20 
#define DB_MISUSE      21 
#define DB_NOLFS       22 
#define DB_AUTH        23 
#define DB_FORMAT      24 
#define DB_RANGE       25 
#define DB_NOTADB      26 
#define DB_NOTICE      27 
#define DB_WARNING     28 
#define DB_ROW         100
#define DB_DONE        101

#endif /* studentdb_h */
