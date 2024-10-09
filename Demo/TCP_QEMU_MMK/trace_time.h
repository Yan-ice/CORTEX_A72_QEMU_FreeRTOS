#ifndef TRACE_TIME_H
#define TRACE_TIME_H

#include <stdint.h>

int startTimer(int timer_id, char* timer_name);
int endTimer(int timer_id);

// #define traceENTER_xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType ) \
//     startTimer(1, "xQueueGenericCreate")

// #define traceRETURN_xQueueGenericCreate( xReturn ) \
//     endTimer(1)

// #define traceENTER_xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition ) \
//     startTimer(2, "xQueueGenericSend")

// #define traceRETURN_xQueueGenericSend( xReturn ) \
//     endTimer(2)

// #define traceENTER_xQueueReceive( xQueue, pvBuffer, xTicksToWait ) \
//     startTimer(3, "xQueueGenericReceive")

// #define traceRETURN_xQueueReceive( xReturn ) \
//     endTimer(3)

// #define traceENTER_vTaskSwitchContext() \
//     startTimer(4, "vTaskSwitchContext")

// #define traceRETURN_vTaskSwitchContext() \
//     endTimer(4)

#endif