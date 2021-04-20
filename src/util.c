#include <stdio.h>
#include <arpa/inet.h>

#include "util.h"

void util_endpoint_to_string(char *buffer, size_t buffer_size, int address_family, const void *endpoint)
{
	switch (address_family)
	{
		case AF_INET:
		{
			const struct sockaddr_in *ep = endpoint;

			char address_buf[INET_ADDRSTRLEN];
			const char *address = inet_ntop(AF_INET, &ep->sin_addr, address_buf, sizeof address_buf);
			const int port = ntohs(ep->sin_port);

			snprintf(buffer, buffer_size, "%s:%d", address, port);

			break;
		}
		case AF_INET6:
		{
			const struct sockaddr_in6 *ep = endpoint;

			char address_buf[INET6_ADDRSTRLEN];
			const char *address = inet_ntop(AF_INET6, &ep->sin6_addr, address_buf, sizeof address_buf);
			const int port = ntohs(ep->sin6_port);

			snprintf(buffer, buffer_size, "[%s]:%d", address, port);

			break;
		}
		default:
		{
			snprintf(buffer, buffer_size, "?");

			break;
		}
	}
}
