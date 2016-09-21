#ifndef FLOW_MONITOR_H
#define FLOW_MONITOR_H

#include "util.h"
#include<sys/types.h>

#define WORD64 unsigned __int64

typedef struct flow_monitor{
    char running_log[64];
    pthread_cond_t cond;
}FLOW_MONITOR;

char *fm_get_running_log_file();
void fm_dump();
int fm_start();

#endif
