#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <unistd.h>

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
    /*Here if do not add any handler to epoll set, 
    * it will run error, but I tried adding regular file fd
    * and fifo fd to epoll, but all failed, so I can only
    * add socket fd, look forword to find reason.
    *----------------------------------------------
    * I used tcp client as the socket fd, and connect to a
    * server, but the connect function will timeout after 75s,
    * so the timer will no use, so I switch to use tcp server,
    * and use accept as the fd.*/
    
    int epollfd, nevents = 0;
    struct epoll_event events[EPOLL_MAXEVENTS];
    struct epoll_event ev;
    int fd, flags;
    
    epollfd = epoll_create(EPOLL_MAXEVENTS);

    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        LOGERR("create socket failed\n");
        exit(-1);
    }
    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint32_t)12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        LOGERR("fail to bind\n");
        exit(-1);
    }

    if (listen(fd, 128) == -1) {
        LOGERR("fail to listen\n");
        exit(-1);
    }
    
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        LOGERR("add fd to epoll failed:%s\n", strerror(errno));
        exit(-1);
    }

    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &sin_size);
    if(connfd == -1){
        if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
            LOGERR("fail to accept:%s\n", strerror(errno));
            exit(-1);
        }
    }

    while (1) {
        nevents = epoll_wait(epollfd, events, EPOLL_MAXEVENTS, EPOLL_WAIT_TIME);
        if (0 == nevents) {
            
            /*timeout, run the function that has been registed*/
            timer.callback();
            
            continue;
        }
    }

    close(fd);
}