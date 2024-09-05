#ifndef MY_MEM_H
#define MY_MEM_H

#include <FreeRTOS.h>
void* memset (void *__s, int __c, size_t __n);

void* memcpy (void *__restrict __dest, const void *__restrict __src, size_t __n);

size_t strlen (const char *__s);

char* strcpy (char *__restrict __dest, const char *__restrict __src);

#endif