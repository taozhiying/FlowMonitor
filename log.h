#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGMSG(fmt, ...)    printf("\033[32m%s line %d MSG: \033[0m"fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOGERR(fmt, ...)    printf("\033[31m%s line %d ERR: \033[0m"fmt, __func__, __LINE__, ##__VA_ARGS__)

#define MAX_LOG_LINE 500
#define LOG_DEL_LINE  300

void log_record(char *log_file, char *log);

#endif
