#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "log.h"
#include "util.h"
#include "protocol.h"

enum
{
	SERVER_LISTEN_PORT = 12345
};

static void *worker_routine(void *arg)
{
	int client_socket_fd = (intptr_t) arg;

	struct sockaddr_storage endpoint;
	socklen_t endpoint_size = sizeof endpoint;

	if (getpeername(client_socket_fd, (struct sockaddr*) &endpoint, &endpoint_size) < 0)
	{
		log_perror("getpeername");
		close(client_socket_fd);
		return NULL;
	}

	char client_string[100];
	util_endpoint_to_string(client_string, sizeof client_string, endpoint.ss_family, &endpoint);

	protocol_process_client_request(client_socket_fd, client_string);

	close(client_socket_fd);

	return NULL;
}

static int create_worker_thread(int client_socket_fd)
{
	void *thread_arg = (void*) (intptr_t) client_socket_fd;

	int error;
	pthread_t id;

	error = pthread_create(&id, NULL, worker_routine, thread_arg);
	if (error)
	{
		log_error_code(error, "pthread_create");
		return -1;
	}

	error = pthread_detach(id);
	if (error)
	{
		log_error_code(error, "pthread_detach");
		return -1;
	}

	return 0;
}

static int create_server_socket()
{
	int socket_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);  // one socket for both IPv4 and IPv6
	if (socket_fd < 0)
	{
		log_perror("socket");
		return -1;
	}

	const int reuse_address = 1;

	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_address, sizeof reuse_address) < 0)
	{
		log_perror("setsockopt");
		close(socket_fd);
		return -1;
	}

	const struct sockaddr_in6 server_endpoint = {
		.sin6_family = AF_INET6,
		.sin6_port = htons(SERVER_LISTEN_PORT),
		.sin6_addr = IN6ADDR_ANY_INIT  // ::
	};

	if (bind(socket_fd, (const struct sockaddr*) &server_endpoint, sizeof server_endpoint) < 0)
	{
		log_perror("bind");
		close(socket_fd);
		return -1;
	}

	if (listen(socket_fd, 64) < 0)
	{
		log_perror("listen");
		close(socket_fd);
		return -1;
	}

	return socket_fd;
}

static int run(int server_socket_fd)
{
	log_info("Listening on port %d", SERVER_LISTEN_PORT);

	for (;;)
	{
		int client_socket_fd = accept(server_socket_fd, NULL, NULL);
		if (client_socket_fd < 0)
		{
			log_perror("accept");
			return -1;
		}

		if (create_worker_thread(client_socket_fd) < 0)
		{
			close(client_socket_fd);
			return -1;
		}
	}

	return 0;
}

int main()
{
	int server_socket_fd = create_server_socket();
	if (server_socket_fd < 0)
	{
		return 1;
	}

	if (run(server_socket_fd) < 0)
	{
		close(server_socket_fd);
		return 1;
	}

	close(server_socket_fd);

	return 0;
}
