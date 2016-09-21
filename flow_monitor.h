#ifndef FLOW_MONITOR_H
#define FLOW_MONITOR_H

#include "util.h"
#include <sys/types.h>
#include <pthread.h>

#define WORD64  long long unsigned int

typedef struct flow_monitor{
    char running_log[64];
    char flow_log[64];
    pthread_cond_t cond;
    WORD64 pre_pps;
    WORD64 next_pps;
    WORD64 pre_bps;
    WORD64 next_bps;
}FLOW_MONITOR;

enum {
    MO_LOG_DBG,
    MO_LOG_MSG,
    MO_LOG_ERR,
};

char *fm_get_running_log_file();
void fm_dump();
int flow_monitor_start();

#endif
