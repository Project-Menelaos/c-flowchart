//
//  main.c
//  JStudentManager
//
//  Created by James Swineson on 12/30/15.
//  Copyright © 2015 James Swineson. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "common.h"
#include "commandinterpreter.h"

int main(int argc, char* argv[]) {

    //srand((unsigned)time(NULL));
    sqlite_load(NULL);
    loop();
    return 0;
}