#ifndef MY_STDIO_H
#define MY_STDIO_H

#include <stddef.h>
#include <extern.h>

void vSendString( const char *s );
void vSendStringISR( const char *s );

void printHex(int num);
void printHexISR(int num);

int sprintf(char *buf, const char *fmt, ...);
int snprintf(char *buf, size_t max_len, const char *fmt, ...);
int fprintf(int fd, const char *fmt, ... );
int printf(const char *__restrict fmt, ...);
int __printf_chk(int __flag, const char *__restrict fmt, ...);


#define print_reg(reg) { \
    int __reg; \
    __asm__ volatile("mrs %0, " #reg "" : "=r" (__reg) ); \
    printf("value of ["#reg"]: %lx\n", __reg); \
}

#endif
