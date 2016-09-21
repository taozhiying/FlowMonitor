#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

#define func_execute_and_return(func,...) \
    do { \
        if (func(__VA_ARGS__) != 0)\
        {\
            LOGERR("func:"#func" is failed\n");\
            return -1;\
        }while(0)


int system1(char *cmd);
int system2(char *cmd);
int get_currnet_date_str(char *date_str, int len);

#endif
