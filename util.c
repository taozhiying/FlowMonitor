#include "util.h"
#include <time.h>

char *now_to_string(char *buf, int size)
{
    time_t t = time(NULL);
    struct tm *p = localtime(&t);
    snprintf(buf, size, "%04d%02d%02d%02d%02d%02d",
             1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
             p->tm_hour, p->tm_min, p->tm_sec);
    return buf;
}

