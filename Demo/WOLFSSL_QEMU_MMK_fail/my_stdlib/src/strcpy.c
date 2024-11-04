#include "my_mem.h"


char *__strcpy_chk (char *__restrict __dest, const char *__restrict __src)
{
    char* dest_c = (char*)__dest;
    char* src_c = (char*)__src;
    while(*src_c != '\0'){
        *dest_c = *src_c;
        src_c++;
        dest_c++;
    };
    *dest_c = '\0';
    return __src;
}

char *strcpy (char *__restrict __dest, const char *__restrict __src) {
    return __strcpy_chk(__dest,__src);
}

char *__strncpy_chk (char *__restrict __dest, const char *__restrict __src, size_t n)
{
    char* dest_c = (char*)__dest;
    char* src_c = (char*)__src;
    while(*src_c != '\0' && n > 1){
        *dest_c = *src_c;
        src_c++;
        dest_c++;
        n--;
    };
    *dest_c = '\0';
    return __src;
}

char *strncpy (char *__restrict __dest, const char *__restrict __src, size_t n)
{
    return __strncpy_chk (__dest,__src,n);
}