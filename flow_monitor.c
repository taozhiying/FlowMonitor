#include "log.h"
#include "flow_monitor.h"

#define RUNNING_LOG_FILE "./run_log"


static FLOW_MONITOR s_flow_monitor;

int fm_init()
{
    pthread_cond_init(&(s_flow_monitor.cond), NULL);

    snprintf(s_flow_monitor.running_log, sizeof(s_flow_monitor.running_log), RUNNING_LOG_FILE);  

    return 0;
}

char *fm_get_running_log_file()
{
    return s_flow_monitor.running_log;
}

void fm_dump()
{
    LOGMSG("---------running info----------");
}

void *fm_thread()
{
    WORD64 pre_pps;
    WORD64 next_pps;
    WORD64 pre_bps;
    WORD64 next_bps;

    pthread_detach(pthread_self());

    while(1) {
        
    }
}

int fm_init_thread()
{
    pthread_t tid = 0;

    if (pthread_create(&tid, NULL, fm_thread, NULL)) {
        LOGERR("fail to pthread_create\n");
        exit(-1);
    }

    return 0;
}

int flow_monitor_start()
{
    func_execute_and_return(fm_init);
    func_execute_and_return(fm_init_thread);

    /*just for test*/
    while(1) {
        sleep(60);
    }
}
