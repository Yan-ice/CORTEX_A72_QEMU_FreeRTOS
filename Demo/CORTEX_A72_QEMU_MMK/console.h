#include <FreeRTOS.h>

void vSendString( const char *s );
void vSendStringISR( const char *s );

void printHex(int num);
void printHexISR(int num);

int Test_sprintf(char *buf, const char *fmt, ...);

#define sprintf Test_sprintf


#define print_reg(reg) \
{   \
    vSendString("value of ["#reg"]: "); \
    int __reg; \
    __asm__ volatile("mrs %0, " #reg "" : "=r" (__reg) ); \
    printHexISR(__reg); \
    vSendString("\n"); \
}