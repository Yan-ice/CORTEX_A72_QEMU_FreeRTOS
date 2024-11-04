#include "my_mem.h"

char 	*strstr (const char *haystack, const char *needle) {
    // 如果 needle 为空，则返回 haystack
    if (*needle == '\0') {
        return (char *)haystack;
    }

    // 遍历 haystack 字符串
    for (; *haystack != '\0'; haystack++) {
        // 比较 haystack 的当前字符和 needle 的第一个字符
        if (*haystack == *needle) {
            const char *h = haystack;   // 记录当前 haystack 的位置
            const char *n = needle;      // 记录 needle 的位置

            // 比较 needle 的剩余字符
            while (*n != '\0' && *h == *n) {
                h++;
                n++;
            }

            // 如果 needle 的所有字符都匹配
            if (*n == '\0') {
                return (char *)haystack; // 返回匹配的起始位置
            }
        }
    }
    return NULL; // 未找到匹配的子字符串
}
char    *strnstr(const char * haystack, const char * needle, size_t len) {
    // 如果 needle 为空，则返回 haystack
    if (*needle == '\0') {
        return (char *)haystack;
    }

    // 遍历 haystack 字符串
    for (; *haystack != '\0'; haystack++) {

        len--;
        if(len == 0) return NULL;

        // 比较 haystack 的当前字符和 needle 的第一个字符
        if (*haystack == *needle) {
            const char *h = haystack;   // 记录当前 haystack 的位置
            const char *n = needle;      // 记录 needle 的位置

            // 比较 needle 的剩余字符
            while (*n != '\0' && *h == *n) {
                h++;
                n++;
            }

            // 如果 needle 的所有字符都匹配
            if (*n == '\0') {
                return (char *)haystack; // 返回匹配的起始位置
            }
        }
    }
    return NULL; // 未找到匹配的子字符串
}
