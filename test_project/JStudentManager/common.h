//
//  common.h
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#ifndef common_h
#define common_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sqlite3.h"

#ifdef __STDC_NO_ATOMICS__
#define _Atomic volatile
#else
#include <stdatomic.h>
#endif

// bstring library
#include "../bstrlib/bstrlib.h"
//#include "../bstrlib/utf8util.h"
#include "../bstrlib/bstraux.h"
// uncommenting the following file will disable unsafe C calls
//#include "../bstrlib/bsafe.h"

#define new(X, ...) (X)malloc((__VA_ARGS__ +1) * sizeof(X))
#define gets_s(X, ...) gets(X)

// dynamically run sprintf, possible memory leak
#define dsprintf(...) bstr2cstr(bformat(__VA_ARGS__), ' ')

#include "../ctrycatch/ctrycatch.h"

typedef char* string;
typedef sqlite3 database;

#define CAT(a, ...) CTRYCATCH_PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define MAX_ORG_NAME_LEN 80
#define MAX_STU_NAME_LEN 80
#define STU_NAME_FMT "%[^\n]80s"

#define clearbuffer(x) fseek(x,0,SEEK_END)

#endif /* common_h */
