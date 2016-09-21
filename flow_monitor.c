#include "log.h"
#include "flow_monitor.h"
#include <signal.h>
#include	<sys/socket.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>


#define RUNNING_LOG_FILE "./run_log"


static FLOW_MONITOR s_flow_monitor;
#define PRE_PPS s_flow_monitor.pre_pps
#define NEXT_PPS s_flow_monitor.next_pps
#define PRE_BPS s_flow_monitor.pre_bps
#define NEXT_BPS s_flow_monitor.next_bps

void fm_record(int type, char *fmt, ...)
{
    char log[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(log, sizeof(log), fmt, ap);
    va_end(ap);

    if (type == MO_LOG_ERR) {
        LOGERR("%s", log);
    }

    log_record(s_flow_monitor.running_log, log);
}

void fm_alarm_handler()
{
    char time_str[128] = {0};

    if (PRE_PPS > NEXT_PPS) {
        PRE_PPS = 0;
        NEXT_PPS = 0;
    }

    if (PRE_BPS > NEXT_BPS) {
        PRE_BPS = 0;
        NEXT_BPS = 0;
    }

    get_currnet_date_str(time_str, sizeof(time_str));
    printf("%s, %llu pps, %.2f kbps\n", time_str, NEXT_PPS - PRE_PPS, (float)(NEXT_BPS - PRE_BPS)/1024);

    PRE_PPS = NEXT_PPS;
    PRE_BPS = NEXT_BPS;

    alarm(1);
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
    unsigned char buffer[2048] = {0};
    int soc_fd;
    int data_size;
    struct sockaddr saddr;
    int saddr_size;

    pthread_detach(pthread_self());

    soc_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soc_fd < 0) {
        fm_record(MO_LOG_ERR, "failed to create socket\n");
        return NULL;
    }  

    while(1) {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(soc_fd, buffer, sizeof(buffer), 0, &saddr ,(socklen_t*)&saddr_size);
        if (data_size < 0) {
            fm_record(MO_LOG_ERR, "recv length 0\n");
            continue;
        } else {
            data_size -= 14;  //minus the length of eth head
            __sync_fetch_and_add(&NEXT_PPS, 1);
            __sync_fetch_and_add(&NEXT_BPS, data_size);
       }
    }
}

int fm_init()
{
    pthread_cond_init(&(s_flow_monitor.cond), NULL);
    snprintf(s_flow_monitor.running_log, sizeof(s_flow_monitor.running_log), RUNNING_LOG_FILE);  
    signal(SIGALRM, fm_alarm_handler);

    PRE_PPS = 0;
    NEXT_PPS = 0;
    PRE_BPS = 0;
    NEXT_BPS = 0;

    return 0;
}

int fm_init_thread()
{
    pthread_t tid = 0;

    if (pthread_create(&tid, NULL, fm_thread, NULL)) {
        fm_record(MO_LOG_ERR, "fail to pthread_create\n");
        return -1;
    }
    
    alarm(1);

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
