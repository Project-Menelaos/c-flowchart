//
//  commandinterpreter.c
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "commandinterpreter.h"

typedef struct {
	char *command;
	int(*eval)(char *s);
	char *help;
} cmd_dict;

cmd_dict commands[] = {
	{ "CLEAR",	cls,		"" },
	{ "CLS",	cls,		"Clears the screen." },
	{ "EXIT",	quit,		"Quits this software (command interpreter)." },
	{ "H",		help,		"" },
	{ "HELP",	help,		"Provides Help information for commands available." },
	{ "MAN",	help,		"" },
	{ "QUIT",	quit,		"" },
	{ "UNAME",	version,	"" },
	{ "VER",	version,	"Displays the software version." },
	{ "VERSION",version,	"" },
    { "!DBOPEN", sqlite_load, "" },
    { "!DBCLOSE", sqlite_save, "" },
    { "!DBPSTU", sqlite_query_all_students, "" },
    { "!DBPORG", sqlite_query_all_orgs, "" },
    { "!DBLOAD", sqlite_to_list, "" },
    { "!PORG", list_query_all_orgs, "" },
    { "DIR", display_orgtree, "Display organization list." },
    { "LS", display_orgtree, "" },
    { "STU", display_student, "Display student list."},
    { "!SQL", sql, "" },
    { "ADDORG", org_add, "Add an orginzation" },
    { "DELORG", org_del, "Removes an orginzation (and everything under it)" },
    { "MODORG", org_modify, "Modify an orginzation" },
    { "FINDORG", org_search, "Search orginzations" },
    { "ADDSTU", stu_add, "Add a student" },
    { "DELSTU", stu_del, "Delete a student" },
    { "MODSTU", stu_modify, "Modify a student" },
    { "FINDSTU", stu_search, "Search students" },
};

//static bool mute = false;

bool is_arg_present(char *arg) {
	if (arg == NULL) return false;
	while (*arg != 0 && isspace(*arg)) arg++;
	if (*arg == 0) return false;
	return true;
}

int help(char *arg) {
	for (int i = 0; i<sizeof commands / sizeof commands[0]; ++i) {
		if (strlen(commands[i].help)) printf("%s\t\t%s\n", commands[i].command, commands[i].help);
	}
	return EXIT_SUCCESS;
}

int version(char *arg) {
	puts("JStudentManager\n(c)2015 James Swineson. All rigits reserved.\nPowered by libJInterpreter.\n");
	return EXIT_SUCCESS;
}

int cls(char *arg) {
#ifdef _WIN32
    system("@cls");
#else
    system("clear 2>/dev/null");
#endif
	puts("");
	return EXIT_SUCCESS;
}

int quit(char *arg) {
	puts("Quitting...");
    sqlite_save(NULL);
	return EXIT_FAILURE;
}

int runcommand(char *s)
{
	char temp[20];
	if (sscanf(s, "%20s", temp) != EOF) {
		temp[19] = 0;
		for (int i = 0; i < strlen(temp); ++i) temp[i] = toupper(temp[i]);
		for (int i = 0; i < sizeof commands / sizeof commands[0]; ++i) {
			if (strcmp(temp, commands[i].command) == 0) {
				char *t = s + strlen(commands[i].command);
				if (isspace(*t)) t++;
				if ( *t == 0 ) t = NULL;
                int ret;
                try {
                    ret = (*(commands[i].eval))(t);
                } catch() {
                    fprintf(stderr, "Error: %s\n", __ctrycatch_exception_message_exists ? __ctrycatch_exception_message : "");
                }
                return ret;
			}
		}
		sscanf(s, "%20s", temp);
		printf("'%s' is not recognized as an internal command.\n", temp);
	}
	return EXIT_SUCCESS;
}

int loop()
{
#ifdef _WIN32
    system("chcp 65001");
#endif
    cls(NULL);
	char cmd[1024 + 1] = { 0 };
	do {
		printf("JStudentManager >");
	} while (gets_s(cmd, 1024)!= NULL && !runcommand(cmd));
	return EXIT_SUCCESS;
}