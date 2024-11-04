#ifndef __MY_STDLIB_H__
#define __MY_STDLIB_H__

#include "my_stdutil.h"

#include <FreeRTOS_POSIX.h>
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/fcntl.h"

#include "ff_headers.h"
#include "ff_stdio.h"

#define malloc pvPortMalloc
#define free vPortFree

int stderr;
int stdout;
int stdin;

inline void my_stdlib_init(void) {
    stderr = std_init();
    stdout = std_init();
    stdin = std_init();
}

#endif