/*
 * Amazon FreeRTOS+POSIX V1.0.4
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
* for aarch64 QEMU.
*/

/**
 * @file FreeRTOS_POSIX_portable.h
 * @brief Port-specific configuration of FreeRTOS+POSIX.
 */

#ifndef _FREERTOS_POSIX_PORTABLE_H_
#define _FREERTOS_POSIX_PORTABLE_H_

/**
 * @name The FreeRTOS task name given to pthreads.
 */
/**@{ */
#ifndef posixconfigPTHREAD_TASK_NAME
    #define posixconfigPTHREAD_TASK_NAME    "pthread" /**< Task name. */
#endif
/**@} */

/**
 * @name the FreeRTOS timer name given to POSIX timers.
 */
/**@{ */
#ifndef posixconfigTIMER_NAME
    #define posixconfigTIMER_NAME    "timer" /**< Timer name. */
#endif
/**@} */

/**
 * @name Defaults for POSIX message queue implementation.
 */
/**@{ */
#ifndef posixconfigMQ_MAX_MESSAGES
    #define posixconfigMQ_MAX_MESSAGES    10 /**< Maximum number of messages in an mq at one time. */
#endif

#ifndef posixconfigMQ_MAX_SIZE
    #define posixconfigMQ_MAX_SIZE    128 /**< Maximum size (in bytes) of each message. */
#endif
/**@} */

/**
 * @name POSIX implementation-dependent constants usually defined in limits.h.
 *
 * They are defined here to provide portability between platforms.
 */
/**@{ */
#ifndef PTHREAD_STACK_MIN
    #define PTHREAD_STACK_MIN    configMINIMAL_STACK_SIZE * sizeof( StackType_t ) /**< Minimum size in bytes of thread stack storage. */
#endif
#ifndef NAME_MAX
    #define NAME_MAX             64                                               /**< Maximum number of bytes in a filename (not including terminating null). */
#endif
#ifndef SEM_VALUE_MAX
    #define SEM_VALUE_MAX        0x7FFFU                                          /**< Maximum value of a sem_t. */
#endif
/**@} */

#include <sys/types.h>
#include <sched.h>

#define posixconfigENABLE_CLOCK_T                0 /**< clock_t in sys/types.h */
#define posixconfigENABLE_CLOCKID_T              0 /**< clockid_t in sys/types.h */
#define posixconfigENABLE_MODE_T                 0 /**< mode_t in sys/types.h */
#define posixconfigENABLE_PID_T                  1 /**< pid_t in sys/types.h */


#define posixconfigENABLE_PTHREAD_ATTR_T         1 /**< pthread_attr_t in sys/types.h */
#define posixconfigENABLE_PTHREAD_COND_T         1 /**< pthread_cond_t in sys/types.h */
#define posixconfigENABLE_PTHREAD_CONDATTR_T     1 /**< pthread_condattr_t in sys/types.h */
#define posixconfigENABLE_PTHREAD_MUTEX_T        1 /**< pthread_mutex_t in sys/types.h */
#define posixconfigENABLE_PTHREAD_MUTEXATTR_T    1 /**< pthread_mutexattr_t in sys/types.h */
#define posixconfigENABLE_PTHREAD_T              1 /**< pthread_t in sys/types.h */
#define posixconfigENABLE_PTHREAD_BARRIER_T      1 /**< pthread_barrier_t in sys/types.h */

#define posixconfigENABLE_SSIZE_T                0 /**< ssize_t in sys/types.h */
#define posixconfigENABLE_TIME_T                 0 /**< time_t in sys/types.h */
#define posixconfigENABLE_TIMER_T                0 /**< timer_t in sys/types.h */
#define posixconfigENABLE_USECONDS_T             1 /**< useconds_t in sys/types.h */

#define posixconfigENABLE_TIMESPEC               0 /**< struct timespec in time.h */
#define posixconfigENABLE_ITIMERSPEC             1 /**< struct itimerspec in time.h */

#define posixconfigENABLE_SEM_T                  1 /**< struct sem_t in semaphore.h */

#define posixconfigENABLE_SCHED_PARAM            0 /**< sched_param in sched.h */

#endif /* _FREERTOS_POSIX_PORTABLE_H_ */
