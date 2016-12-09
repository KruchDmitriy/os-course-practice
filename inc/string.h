#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

size_t strlen(const char *str);
int strcmp(char *s, char *t);
void *memcpy(void *dst, const void *src, size_t size);
void *memset(void *dst, int fill, size_t size);

#endif /*__STRING_H__*/
