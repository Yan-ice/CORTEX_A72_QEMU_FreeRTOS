#include "trace_time.h"

uint64_t time_d[64];
int trace_nest;

int startTimer(int timer_id, char* timer_name){
    int a;
    for(a = 0;a<trace_nest;a++) vSendString("    ");
    trace_nest++;

    char buf[80];
    sprintf(buf, "#### trace_begin %d [%s]:\n", timer_id, timer_name);
    vSendStringISR(buf);
    
    asm volatile("mrs %0, cntvct_el0" : "=r" (time_d[timer_id]) );
    
}

int endTimer(int timer_id){

    uint64_t t_end;
    asm volatile("mrs %0, cntvct_el0" : "=r" (t_end) );

    trace_nest--;
    int a;
    for(a = 0;a<trace_nest;a++) vSendString("    ");

    char buf[80];
    sprintf(buf, "#### trace_end %d [%d cycle]\n", timer_id, t_end - time_d[timer_id]);
    vSendStringISR(buf);
}
