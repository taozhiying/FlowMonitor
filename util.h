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
            }  \
        }while(0)

typedef struct __jdis_date__ {
    int sec;
    int min;
    int hour;
    int day;
    int mon;
    int year;
} JDIS_DATE;

int system1(char *cmd);
int system2(char *cmd);
int get_currnet_date_str(char *date_str, int len);
char *now_to_string(char *buf, int size);


#endif
