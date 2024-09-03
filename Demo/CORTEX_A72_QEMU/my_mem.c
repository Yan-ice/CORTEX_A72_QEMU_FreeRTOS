#include "my_mem.h"

void* memset (void *__s, int __c, size_t __n){
    char *b = __s;
    while(__n > 0){
        *b = __c;
        b++;
        __n--;
    }
    return __s;
}

void *memcpy (void *__dest, const void *__src, size_t __n) {
    char* dest_c = (char*)__dest;
    char* src_c = (char*)__src;
    while(__n > 0){
        *dest_c = *src_c;
        __n--;
    }
    return __src;
}

