#include <signal.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include "flow_monitor.h"
#include "timer.h"


/*the interval to calculate flow rate, 
*metric with millisecond, default 1 sec*/
#define CALC_FLOW_INTERVAL 1000

static FLOW_MONITOR flow_monitor;

#define PRE_PPS flow_monitor.pre_pps
#define NEXT_PPS flow_monitor.next_pps
#define PRE_BPS flow_monitor.pre_bps
#define NEXT_BPS flow_monitor.next_bps

void fm_calc_timeout()
{
    char str[512] = {0};
    WORD64 now_pps, now_bps;

    /*copy the value first, then do calculation,
    * preventing lock the NEXT_PPS and NEXT_BPS too long,
    * then the socket can not get them*/
    pthread_mutex_lock(&flow_monitor.calc_lock);
    now_pps = NEXT_PPS;
    now_bps = NEXT_BPS;
    pthread_mutex_unlock(&flow_monitor.calc_lock);

    /*in case the word64 turn over*/
    if (PRE_PPS > now_pps) {
        pthread_mutex_lock(&flow_monitor.calc_lock);
        PRE_PPS = 0;
        NEXT_PPS = 0;
        pthread_mutex_unlock(&flow_monitor.calc_lock);
    }
    if (PRE_BPS > now_pps) {
        pthread_mutex_lock(&flow_monitor.calc_lock);
        PRE_BPS = 0;
        NEXT_BPS = 0;
        pthread_mutex_unlock(&flow_monitor.calc_lock);
    }

    snprintf(str, sizeof(str), "%llu pps, %.2f kbps\n", now_pps - PRE_PPS, (float)(now_bps - PRE_BPS)/1024);
    printf("%s", str);

    PRE_PPS = now_pps;
    PRE_BPS = now_bps;
}


void *flow_monitor_loop()
{
    unsigned char buffer[2048] = {0};
    int soc_fd;
    int data_size;
    struct sockaddr saddr;
    int saddr_size;

    soc_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soc_fd < 0) {
        LOGERR("failed to create socket:%s\n", strerror(errno));
        return NULL;
    }  

    while(1) {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(soc_fd, buffer, sizeof(buffer), 0, &saddr ,(socklen_t*)&saddr_size);
        if (data_size < 0) {
            continue;
        } else {
            data_size -= 14;  //minus the length of eth head

            pthread_mutex_lock(&flow_monitor.calc_lock);
            __sync_fetch_and_add(&NEXT_PPS, 1);
            __sync_fetch_and_add(&NEXT_BPS, data_size);
            pthread_mutex_unlock(&flow_monitor.calc_lock);
       }
    }
}

int fm_init()
{
    pthread_mutex_init(&flow_monitor.timer_lock, NULL);
    pthread_mutex_init(&flow_monitor.calc_lock, NULL);

    PRE_PPS = 0;
    NEXT_PPS = 0;
    PRE_BPS = 0;
    NEXT_BPS = 0;

    /*set timer*/
    timer_callback_register(fm_calc_timeout);
    timer_set_interval(CALC_FLOW_INTERVAL);
    
    return 0;
}

int flow_monitor_start()
{
    fm_init();

    pthread_t tid = 0;
    if (pthread_create(&tid, NULL, timer_thread, NULL)) {
        LOGERR("create timer thread failed!\n");
        return -1;
    }

    flow_monitor_loop();

    pthread_mutex_destroy(&flow_monitor.timer_lock);
    pthread_mutex_destroy(&flow_monitor.calc_lock);

    return 0;
}
