#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h> 

#include "timer.h"
#include "flow_monitor.h"

static TIMER timer;

void timer_callback_register(void (* func))
{
    timer.callback = func;
}

void timer_set_interval(int interval)
{
    timer.interval = interval;
}

#define EPOLL_MAXEVENTS 1
#define EPOLL_WAIT_TIME timer.interval

void *timer_thread()
{
    int epollfd, nevents = 0;
    struct epoll_event events[EPOLL_MAXEVENTS];
    struct epoll_event ev;
    int fd, flags;

    /*Here if do not add any handler to epoll set, 
    * it will run error, but I tried adding regular file fd
    * and fifo fd to epoll, but all failed, so I can only
    * add socket fd, look forword to find reason.*/
    
    epollfd = epoll_create(EPOLL_MAXEVENTS);

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        LOGERR("create socket failed\n");
        exit(-1);
    }
    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in destAddr;
    bzero((void *)&destAddr,sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(12345);
    destAddr.sin_addr.s_addr = ntohl(0x08080808);

    /*start connect*/
    if ((connect(fd, (struct sockaddr *)&destAddr, sizeof(struct sockaddr)) < 0) && (errno != EINPROGRESS)) {  
        LOGERR("connect failed\n");
        exit(-1);
    }
    
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        LOGERR("add fd to epoll failed:%s\n", strerror(errno));
        exit(-1);
    }

    while (1) {
        nevents = epoll_wait(epollfd, events, EPOLL_MAXEVENTS, EPOLL_WAIT_TIME);
        if (0 == nevents) {
            
            /*timeout, run the function that has been registed*/
            timer.callback();
            
            continue;
        }
    }
}