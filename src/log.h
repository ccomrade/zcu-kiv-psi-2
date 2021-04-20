#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

void log_info(const char *format, ...);
void log_info_v(const char *format, va_list args);

void log_error(const char *format, ...);
void log_error_v(const char *format, va_list args);

void log_error_code(int code, const char *format, ...);
void log_error_code_v(int code, const char *format, va_list args);

void log_perror(const char *format, ...);
void log_perror_v(const char *format, va_list args);

#endif
