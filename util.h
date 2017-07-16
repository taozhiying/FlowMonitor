#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGMSG(fmt, ...)    printf("\033[32m%s line %d MSG: \033[0m"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOGERR(fmt, ...)    printf("\033[31m%s line %d ERR: \033[0m"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define func_execute_and_return(func,...) \
    do { \
            if (func(__VA_ARGS__) != 0)\
            {\
                LOGERR("func:"#func" is failed\n");\
                return -1;\
            }  \
        }while(0)


char *now_to_string(char *buf, int size);


#endif
