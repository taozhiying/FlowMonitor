#ifndef __TIMER_H__
#define __TIMER_H__

#include "util.h"

typedef struct _timer_ {
    void (* callback)();
    int interval;
} TIMER;

void timer_callback_register(void (* func));
void *timer_thread();
void timer_set_interval(int interval);

#endif