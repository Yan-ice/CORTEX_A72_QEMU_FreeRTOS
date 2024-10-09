#ifndef MY_MEM_H
#define MY_MEM_H

#include <stddef.h>

void* memset (void *__s, int __c, size_t __n);
void* memcpy (void *__restrict __dest, const void *__restrict __src, size_t __n);
void* memmove (void *__restrict __dest, const void *__restrict __src, size_t __n);
int memcmp (const void *__restrict __ptr1, const void *__restrict __ptr2, size_t __n);

size_t strlen (const char *__s);
size_t	 strnlen (const char *, size_t);

char* strcpy (char *__restrict __dest, const char *__restrict __src);
char 	*strncpy (char *__restrict, const char *__restrict, size_t);

int	 strcmp (const char *, const char *);
int	 strncmp (const char *, const char *, size_t);

//append str2 after str1
char 	*strcat (char *__restrict, const char *__restrict);
//append n chars most.
char 	*strncat (char *__restrict, const char *__restrict, size_t);

//find first char in str
char 	*strchr (const char *, int);
//find last char in str
char 	*strrchr (const char *, int);

//find first substring in str
char 	*strstr (const char *, const char *);
char    *strnstr(const char *, const char *, size_t);

#endif
