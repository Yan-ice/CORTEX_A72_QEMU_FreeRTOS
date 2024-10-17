#include "my_stdio.h"
#include "stdarg.h"

#include "FreeRTOS.h"

void vSendString( const char *s )
{
	size_t i = 0;

	portENTER_CRITICAL();
	int cnt = 100;
	while (s[i] != '\0' && cnt > 0) {
		uartputc_sync( s[i] );
		cnt--;
		i++;
	}
	portEXIT_CRITICAL();
}

void vSendStringISR( const char *s )
{
	size_t i = 0;

	//portENTER_CRITICAL();
	int cnt = 100;
	while (s[i] != '\0' && cnt > 0) {
		uartputc_sync( s[i] );
		cnt--;
		i++;
	}
	//portEXIT_CRITICAL();
}

void intToHex(unsigned int num, char *hexStr) {
    // 十六进制字符表
    const char hexDigits[] = "0123456789ABCDEF";
    int i = 0;
    
    // 处理特殊情况：整数为 0
    if (num == 0) {
        hexStr[i++] = '0';
        hexStr[i] = '\0';
        return;
    }

    // 将整数转换为十六进制字符
    while (num != 0) {
        int remainder = num % 16;
        hexStr[i++] = hexDigits[remainder];
        num /= 16;
    }

    // 添加字符串终止符
    hexStr[i] = '\0';

    // 反转字符串以获得正确的十六进制表示
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = hexStr[j];
        hexStr[j] = hexStr[k];
        hexStr[k] = temp;
    }
}

void printHexISR(int num) {
    vSendStringISR("0x");
    char hex[30];
    intToHex(num, hex);
    vSendStringISR(hex);
    vSendStringISR("\n");
}

void printHex(int num) {
    vSendString("0x");
    char hex[30];
    intToHex(num, hex);
    vSendString(hex);
    vSendString("\n");
}

 #define ZEROPAD    1       /* pad with zero */
 #define SIGN   2       /* unsigned/signed long */
 #define PLUS   4       /* show plus */
 #define SPACE  8       /* space if plus */
 #define LEFT   16      /* left justified */
 #define SPECIAL    32      /* 0x */
 #define LARGE  64      /* use 'ABCDEF' instead of 'abcdef' */

 int _div(long* n,unsigned base)
 {
     int __res; 
         __res = ((unsigned long) *n) % (unsigned) base; 
         *n = ((unsigned long) *n) / (unsigned) base; 
         return __res;
 }

#define do_div(n,base) _div(&n,base)/*({ \
    int __res; \
    __res = ((unsigned long) n) % (unsigned) base; \
    n = ((unsigned long) n) / (unsigned) base; \
    __res; })*/


size_t Test_strnlen(const char *s, size_t count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc);
    return sc - s;
}



 static inline int isdigit(int ch)
 {
    return (ch >= '0') && (ch <= '9');
 }



 static int skip_atoi(const char **s)
 {
    int i = 0;

    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
 }



 static char *Test_number(char *str, long num, int base, int size, int precision,
    int type)
 {
    char c, sign, tmp[66];
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i;

    if (type & LARGE)
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL) {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    i = 0;
    if (num == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
        {
            tmp[i++] = digits[do_div(num, base)];
        }
    }

    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';
    if (sign)
        *str++ = sign;
    if (type & SPECIAL) {
        if (base == 8)
            *str++ = '0';
        else if (base == 16) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))
        while (size-- > 0)
            *str++ = c;
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    return str;
 }



//max_len not used.
int Test_vsnprintf(char *buf, size_t max_len, const char *fmt, va_list args)
{
    int len;
    unsigned long num;
    int i, base;
    char *str;
    const char *s;

    int flags;      /* flags to Test_number() */

    int field_width;    /* width of output field */
    int precision;      /* min. # of digits for integers; max
                   Test_number of chars for from string */
    int qualifier;      /* 'h', 'l', or 'L' for integer fields */

    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        /* process flags */
        flags = 0;
          repeat:
        ++fmt;      /* this also skips first '%' */
        switch (*fmt) {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*') {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*') {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) {
        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char)va_arg(args, int);
            while (--field_width > 0)
                *str++ = ' ';
            continue;

        case 's':
            s = va_arg(args, char *);
            len = Test_strnlen(s, precision);

            if (!(flags & LEFT))
                while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            continue;

        case 'p':
            if (field_width == -1) {
                field_width = 2 * sizeof(void *);
                flags |= ZEROPAD;
            }
            str = Test_number(str,
                     (unsigned long)va_arg(args, void *), 16,
                     field_width, precision, flags);
            continue;

        case 'n':
            if (qualifier == 'l') {
                long *ip = va_arg(args, long *);
                *ip = (str - buf);
            } else {
                int *ip = va_arg(args, int *);
                *ip = (str - buf);
            }
            continue;

        case '%':
            *str++ = '%';
            continue;

            /* integer Test_number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            continue;
        }
        if (qualifier == 'l')
            num = va_arg(args, unsigned long);
        else if (qualifier == 'h') {
            num = (unsigned short)va_arg(args, int);
            if (flags & SIGN)
                num = (short)num;
        } else if (flags & SIGN)
            num = va_arg(args, int);
        else
            num = va_arg(args, unsigned int);
        str = Test_number(str, num, base, field_width, precision, flags);
    }
    *str = '\0';
    return str - buf;
}

int snprintf(char *buf, size_t max_num, const char *fmt, ...)
{
    //记录fmt对应的地址
    va_list args;
    int val;
    //得到首个%对应的字符地址
    va_start(args, fmt);
    val = Test_vsnprintf(buf, max_num, fmt, args);
    va_end(args);
    return val;
}

int sprintf(char *buf, const char *fmt, ...)
{
    //记录fmt对应的地址
    va_list args;
    int val;
    //得到首个%对应的字符地址
    va_start(args, fmt);
    val = Test_vsnprintf(buf, 0x7fffffff, fmt, args);
    va_end(args);
    return val;
}

#include <FreeRTOS_POSIX/fcntl.h>
int fprintf(int fd, const char *fmt, ... ) 
{
    va_list args;

    int val;
    char buf[0x200];
    va_start(args, fmt);
    val = Test_vsnprintf(buf, 0x200, fmt, args);
    va_end(args);
    write(fd,buf,val);
    return val;
}

int printf(const char *__restrict fmt, ...)
{
    va_list args;

    char buf[0x200];
    //记录fmt对应的地址
    //得到首个%对应的字符地址
    va_start(args, fmt);
    int val = Test_vsnprintf(buf, 0x200, fmt, args);
    va_end(args);
    if(val > 200){
        vSendString("buffer overflow.\n");;
    }
    vSendString(buf);
    return val;
}

int __printf_chk(int __flag, const char *__restrict fmt, ...)
{
    char buf[0x200];
    //记录fmt对应的地址
    va_list args;
    //得到首个%对应的字符地址
    va_start(args, fmt);
    int val = Test_vsnprintf(buf, 0x7fffffff, fmt, args);
    va_end(args);
    if(val > 200){
        vSendString("buffer overflow.\n");;
    }
    vSendString(buf);
    return val;
}

