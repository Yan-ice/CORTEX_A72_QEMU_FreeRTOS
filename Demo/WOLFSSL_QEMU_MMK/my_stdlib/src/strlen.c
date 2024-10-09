#include "my_mem.h"

size_t strlen (const char *__s){
    size_t len = 0;
    while(*__s != '\0'){
        len++;
        *__s++;
    }
    return len;
}

size_t strnlen (const char *__s, size_t n){
    size_t len = 0;
    while(*__s != '\0'){
        len++;
        *__s++;
    }
    return len > n ? n : len;
}