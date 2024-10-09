#include "my_mem.h"


void* __memset_chk (void *__s, int __c, size_t __n){
    char *b = __s;
    while(__n > 0){
        *b = __c;
        b++;
        __n--;
    }
    return __s;
}

void* memset (void *__s, int __c, size_t __n){
    return __memset_chk(__s,__c,__n);
}


void *__memcpy_chk (void *__restrict __dest, const void *__restrict __src, size_t __n) {
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

void *memcpy (void *__restrict __dest, const void *__restrict __src, size_t __n) {
    return __memcpy_chk(__dest,__src,__n);
}

void* memmove (void *__restrict __dest, const void *__restrict __src, size_t __n) {
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

int memcmp (const void *__restrict __ptr1, const void *__restrict __ptr2, size_t __n) {
    char* ptr1_c = (char*)__ptr1;
    char* ptr2_c = (char*)__ptr2;
    while(__n > 0){
        if(*ptr1_c > *ptr2_c ) {
        	return 1;
        }else if (*ptr1_c < *ptr2_c ) {
        	return -1;
        }
        __n--;
        ptr1_c++;
        ptr2_c++;
    }
    return 0;
}

