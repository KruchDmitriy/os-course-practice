#ifndef __IO_H__
#define __IO_H__

#include <stdarg.h>
#include <stddef.h>

void printf(const char * format, ...);
void vprintf(const char * format, va_list arg);
void snprintf(char * s, size_t n, const char * format, ...);
void vsnprintf(char * s, size_t n, const char * format, va_list arg);

#endif /* __IO_H__ */
