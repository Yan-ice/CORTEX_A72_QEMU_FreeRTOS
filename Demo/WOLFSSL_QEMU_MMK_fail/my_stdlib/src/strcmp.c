#include "my_mem.h"

int	strcmp (const char *__str1, const char *__str2) {
    char* dest_c = (char*)__str1;
    char* src_c = (char*)__str2;
    while(*src_c != '\0'){
        if (*__str1==*__str2) continue;

        if (*__str1 > *__str2) { return 1; }
        else { return -1; }

        src_c++;
        dest_c++;
    };
    return 0;
}
int strncmp (const char *__str1, const char *__str2, size_t n) {
    char* dest_c = (char*)__str1;
    char* src_c = (char*)__str2;
    while(*src_c != '\0' && n > 0){
        if (*__str1==*__str2) continue;

        if (*__str1 > *__str2) { return 1; }
        else { return -1; }

        src_c++;
        dest_c++;
    };
    return 0;
}
