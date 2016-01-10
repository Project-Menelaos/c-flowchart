//
//  commandinterpreter.h
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#pragma once
#ifndef __COMMANDINTERPRETER_H__
#define __COMMANDINTERPRETER_H__

#define COMMAND_MAX_LENGTH 512
#define FILENAME_MAX_LENGTH 1024
#define FILENAME_MAX_LENGTH_CHAR "1024"

int run_command(char *args);
int loop();
int help(char *arg);
int version(char *arg);
int cls(char *arg);
int quit(char *arg);
int save(char *arg);
int load(char *arg);

#include "commands.h"

#endif