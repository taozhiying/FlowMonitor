#ifndef __FLOW_MONITOR_H__
#define __FLOW_MONITOR_H__

#include "util.h"
#include <sys/types.h>
#include <pthread.h>

#define WORD64  long long unsigned int

typedef struct _flow_monitor_ {
    pthread_mutex_t timer_lock;
    pthread_mutex_t calc_lock;
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
