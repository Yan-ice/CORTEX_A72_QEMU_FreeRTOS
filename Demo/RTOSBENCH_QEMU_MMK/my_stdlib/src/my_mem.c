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

void *memcpy (void *__restrict __dest, const void *__restrict __src, size_t __n) {
    char* dest_c = (char*)__dest;
    char* src_c = (char*)__src;
    while(__n > 0){
        *dest_c = *src_c;
        __n--;
        src_c++;
        dest_c++;
    }
    return __src;
}


size_t strlen (const char *__s){
    size_t len = 0;
    while(*__s != '\0'){
        len++;
        *__s++;
    }
    return len;
}

char *strcpy (char *__restrict __dest, const char *__restrict __src)
{
    char* dest_c = (char*)__dest;
    char* src_c = (char*)__src;
    while(src_c != '\0'){
        *dest_c = *src_c;
        src_c++;
        dest_c++;
    };
    *dest_c = '\0';
    return __src;
}