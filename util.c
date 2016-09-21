#include "util.h"

/*  like fuc system(), but it will allso get the return of  the child process
 */
int system1(char *cmd)
{
    int ret = -1;
    
    if (!cmd) {
        LOGERR("param null.\n");
        return -1;
    }
    
    ret = system(cmd);
    if (ret == -1) {
        LOGERR("system1() error. [cmd:%s] [err:%m]\n", cmd);
    } else {
        if (WIFEXITED(ret)) { /* normal exit script ? */
            ret = WEXITSTATUS(ret);
            if (ret != 0) {
                LOGERR("run shell script fail,  [cmd:%s] [exit code: %d]\n", cmd, ret);
            } else {
                LOGMSG("system1 run ok, [cmd : %s]\n", cmd);
            }
        } else {
            ret = WEXITSTATUS(ret);
            LOGMSG("shell script [%s] exit, status = [%d] \n", cmd, ret);
        }
    }
    
    return ret;
} 

int system2(char *cmd)
{
    LOGMSG("run cmd [%s]\n", cmd);
    return system(cmd);
}

int get_currnet_date_str(char *date_str, int len)
{
    JDIS_DATE date = {0};
    if (NULL == jdis_audit_get_curr_date(&date)) {
        return -1;
    }
    snprintf(date_str, len,  "%04d-%02d-%02d %02d:%02d:%02d",
            date.year, date.mon,  date.day, date.hour,  date.min, date.sec);
    return 0;
}

