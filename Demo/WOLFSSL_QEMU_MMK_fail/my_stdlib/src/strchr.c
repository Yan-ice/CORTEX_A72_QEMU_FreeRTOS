#include "my_mem.h"

//find first char in str
char 	*strchr (const char * __str, int c) {
    char *src_t = __str;
    while (src_t != 0) {
        if(src_t == c) {return src_t;}
        src_t++;
    }
    return (char*)0;
}
//find last char in str
char 	*strrchr (const char * __str, int c) {
    char *src_t = __str;
    char *record = (char*)0;
    while (src_t != 0) {
        if(src_t == c) {record = src_t;}
        src_t++;
    }
    return record;
}
