#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"

static void error_code_to_string(char *buffer, size_t buffer_size, int code)
{
	char description_buffer[100];
	const char *description = NULL;

#ifdef _GNU_SOURCE
	description = strerror_r(code, description_buffer, sizeof description_buffer);
#else
	if (strerror_r(code, description_buffer, sizeof description_buffer) == 0)
	{
		description = description_buffer;
	}
#endif

	if (description)
	{
		snprintf(buffer, buffer_size, ": Error code %d (%s)", code, description);
	}
	else
	{
		snprintf(buffer, buffer_size, ": Error code %d", code);
	}
}

static void write_message(char *message)
{
	const size_t length = strlen(message);

	// temporary replace terminating null character with newline
	message[length] = '\n';

	write(STDERR_FILENO, message, length + 1);

	message[length] = '\0';
}

void log_info(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_info_v(format, args);
	va_end(args);
}

void log_info_v(const char *format, va_list args)
{
	char buffer[1024];
	vsnprintf(buffer, sizeof buffer, format, args);

	write_message(buffer);
}

void log_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_error_v(format, args);
	va_end(args);
}

void log_error_v(const char *format, va_list args)
{
	char buffer[1024];
	vsnprintf(buffer, sizeof buffer, format, args);

	write_message(buffer);
}

void log_error_code(int code, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_error_code_v(code, format, args);
	va_end(args);
}

void log_error_code_v(int code, const char *format, va_list args)
{
	const size_t description_buffer_size = 128;

	char buffer[1024];
	vsnprintf(buffer, (sizeof buffer) - description_buffer_size, format, args);

	error_code_to_string(buffer + strlen(buffer), description_buffer_size, code);

	write_message(buffer);
}

void log_perror(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_perror_v(format, args);
	va_end(args);
}

void log_perror_v(const char *format, va_list args)
{
	log_error_code_v(errno, format, args);
}
