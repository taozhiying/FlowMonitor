#include <signal.h>
#include "flow_monitor.h"
#include <unistd.h>
#include <execinfo.h>

#define VERSION "20160920"

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

#define DUMP_2_STDOUT   1
void sigsegv_handler(int sig)
{
    int j, nptrs;
#define SIZE 100
    void *buffer[100];
    char timebuf[64];
    char **strings;
    char *running_log;
    char write_buf[1024] = {0};

    nptrs = backtrace(buffer, SIZE);
    
    fprintf(write_buf,
            "**************************************************************\n"
            "                       recv signal %d                        *\n"
            "*   %s backtrace() returned %d addresses         *\n"
            "**************************************************************\n",
            sig, now_to_string(timebuf, sizeof(timebuf)), nptrs);
    log_record(running_log, write_buf);
    
#if DUMP_2_STDOUT
    fprintf(stderr,
            "**************************************************************\n"
            "                       recv signal %d                        *\n"
            "*   %s backtrace() returned %d addresses         *\n"
            "**************************************************************\n",
            sig, now_to_string(timebuf, sizeof(timebuf)), nptrs);
#endif

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        LOGERR("backtrace_symbols [%m]");
        exit(EXIT_FAILURE);
    }
    
    running_log = fm_get_running_log_file();
    for (j = 0; j < nptrs; j++) {
        fprintf(stderr, "%s\n", strings[j]);
        log_record(running_log, strings[j]);
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
    signal(SIGUSR1, fm_dump);

    return 0;
}

int main(int argc, char **argv)
{
    func_execute_and_return(handle_opt, argc, argv);
    func_execute_and_return(catch_sig_init);
    func_execute_and_return(flow_monitor_start);

    return 0;
}