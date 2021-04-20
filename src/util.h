#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void util_endpoint_to_string(char *buffer, size_t buffer_size, int address_family, const void *endpoint);

#endif
