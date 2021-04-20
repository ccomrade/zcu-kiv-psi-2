#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "protocol.h"
#include "log.h"

static int receive_request(int fd, char *buffer, size_t buffer_size, size_t *request_length)
{
	ssize_t length = read(fd, buffer, buffer_size - 1);  // terminating null character
	if (length < 0)
	{
		log_perror("read");
		return -1;
	}

	buffer[length] = '\0';

	(*request_length) = length;

	return 0;
}

static int send_response(int fd, const char *response, size_t response_length)
{
	ssize_t length = write(fd, response, response_length);
	if (length < 0)
	{
		log_perror("write");
		return -1;
	}

	return 0;
}

static int send_http_response(int fd, const char *status_code, const char *content_type, const char *content)
{
	const size_t content_length = strlen(content);

	char buffer[1024];

	snprintf(buffer, sizeof buffer,
		"HTTP/1.1 %s\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %zu\r\n"
		"\r\n"
		"%s",
		status_code, content_type, content_length, content
	);

	return send_response(fd, buffer, strlen(buffer));
}

static int send_http_bad_request(int fd)
{
	const char *page = "<!DOCTYPE html>\n"
	                   "<html lang=\"en\">\n"
	                   " <head>\n"
	                   "  <meta charset=\"UTF-8\">\n"
	                   "  <title>400 Bad Request</title>\n"
	                   " </head>\n"
	                   " <body>\n"
	                   "  <h1>400 Bad Request</h1>\n"
	                   " </body>\n"
	                   "</html>\n";

	return send_http_response(fd, "400 Bad Request", "text/html", page);
}

static int send_http_not_implemented(int fd)
{
	const char *page = "<!DOCTYPE html>\n"
	                   "<html lang=\"en\">\n"
	                   " <head>\n"
	                   "  <meta charset=\"UTF-8\">\n"
	                   "  <title>501 Not Implemented</title>\n"
	                   " </head>\n"
	                   " <body>\n"
	                   "  <h1>501 Not Implemented</h1>\n"
	                   " </body>\n"
	                   "</html>\n";

	return send_http_response(fd, "501 Not Implemented", "text/html", page);
}

static int send_default_page(int fd, const char *uri, const char *client_name)
{
	char buffer[512];

	snprintf(buffer, sizeof buffer,
		"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		" <head>\n"
		"  <meta charset=\"UTF-8\">\n"
		"  <title>%s</title>\n"
		" </head>\n"
		" <body>\n"
		"  <h2>Hello %s</h2>\n"
		" </body>\n"
		"</html>\n",
		uri, client_name
	);

	return send_http_response(fd, "200 OK", "text/html", buffer);
}

int protocol_process_client_request(int client_socket_fd, const char *client_name)
{
	char request_buffer[1024];
	size_t request_length = 0;

	if (receive_request(client_socket_fd, request_buffer, sizeof request_buffer, &request_length) < 0)
	{
		return -1;
	}

	char method[16];
	char uri[256];
	char http_version[16];

	// parse request line
	if (sscanf(request_buffer, "%15s %255s %15s", method, uri, http_version) != 3)
	{
		send_http_bad_request(client_socket_fd);
		return -1;
	}

	if (strcmp(method, "GET") != 0)
	{
		send_http_not_implemented(client_socket_fd);
		return -1;
	}

	log_info("%s %s %s", client_name, method, uri);

	return send_default_page(client_socket_fd, uri, client_name);
}
