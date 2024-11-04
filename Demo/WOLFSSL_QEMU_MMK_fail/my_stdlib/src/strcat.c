#include "my_mem.h"

//append str2 after str1
char 	*strcat (char *__restrict __dest, const char *__restrict __src){
    char* dest_c = (char*)__dest;
    while(*dest_c != 0) {
        dest_c++;
    }
    strcpy(dest_c, __src);
    return __dest;
}
//append n chars most.
char 	*strncat (char *__restrict __dest, const char *__restrict __src, size_t n) {
    char* dest_c = (char*)__dest;
    while(*dest_c != 0) {
        dest_c++;
    }
    strncpy(dest_c, __src, n);
    return __dest;
}
