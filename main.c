#include <signal.h>
#include "flow_monitor.h"
#include <unistd.h>
#include <execinfo.h>
#include <syslog.h>
#include <sys/file.h>
#include <fcntl.h>

#define VERSION "20170716"

int handle_opt(int argc, char *argv[])
{
    char oc;

    while((oc = getopt(argc, argv, "v")) != -1) {
        switch (oc) {
        case 'v':
            printf("version: [%s]\n", VERSION);
            exit(0);
            break;

        default:
            break;
        }
    }

    return 0;
}

void sigsegv_handler(int sig)
{
    int j, nptrs;
#define SIZE 100
    void *buffer[100];
    char timebuf[64];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    
    fprintf(stdout,
            "**************************************************************\n"
            "                       recv signal %d                        *\n"
            "*   %s backtrace() returned %d addresses         *\n"
            "**************************************************************\n",
            sig, now_to_string(timebuf, sizeof(timebuf)), nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        LOGERR("backtrace_symbols [%m]");
        exit(EXIT_FAILURE);
    }
    
    for (j = 0; j < nptrs; j++) {
        fprintf(stderr, "%s\n", strings[j]);
    }
    
    free(strings);
    exit(1);
}

int catch_sig_init()
{
    signal(SIGSEGV, sigsegv_handler);
    signal(SIGALRM, sigsegv_handler);
    signal(SIGKILL, sigsegv_handler);
    signal(SIGHUP, sigsegv_handler);
    signal(SIGTERM, sigsegv_handler);
    signal(SIGABRT, sigsegv_handler);
    signal(SIGFPE, sigsegv_handler);
    signal(SIGILL, sigsegv_handler);
    signal(SIGQUIT, sigsegv_handler);
    signal(SIGTRAP, sigsegv_handler);
    signal(SIGBUS, sigsegv_handler);
    signal(SIGXCPU, sigsegv_handler);
    signal(SIGXFSZ, sigsegv_handler);

    return 0;
}

#define LOCK_FILE "/tmp/flow_monitor.pid"   
#define LOCK_MODE (S_IRUSR|S_IWUSR|S_IRGRP)
int is_already_running(const char *lock_file, mode_t lock_mode)
{
    int ret, fd;
    char buf[32];
    struct flock fl;

    fd = open(lock_file, O_RDWR|O_CREAT, lock_mode);
    if (fd < 0) {
        LOGERR("is_already_running, open lock file failed!\n");
        exit(1);
    }
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    ret = fcntl(fd, F_SETLK, &fl);
    if (ret) {
        /* already running or some error. */
        close(fd);
        return 1;
    }
    /* O.K. write the pid */
    ret = ftruncate(fd,0);
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)getpid());
    ret = write(fd, buf, strlen(buf) + 1);

    return 0;
}


int main(int argc, char **argv)
{
    handle_opt(argc, argv);
    catch_sig_init();

    if (is_already_running(LOCK_FILE, LOCK_MODE)) {
        LOGERR("the program is already running!\n");
        exit(0);
    }
    
    func_execute_and_return(flow_monitor_start);

    return 0;
}