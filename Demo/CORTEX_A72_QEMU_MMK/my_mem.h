#ifndef MY_MEM_H
#define MY_MEM_H

#include <FreeRTOS.h>

void* memset (void *__s, int __c, size_t __n)
                                __THROW __nonnull ((1));

void *memcpy (void *__restrict __dest, const void *__restrict __src, size_t __n) 
                                __THROW __nonnull ((1, 2));

#endif