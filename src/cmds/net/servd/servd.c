/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Vita Loginova
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/l3/ipv4/ip.h>
#include <sys/socket.h>
#include <net/inetdevice.h>
#include <cmd/web_server.h>
#include <cmd/servd.h>
#include <embox/web_service.h>
#include <kernel/task.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

static int web_server_started = 0;
static int web_server_task;

static void welcome_message(void) {
	/* FIXME cheat to get local ip */
	struct in_addr local_addr;
	struct in_device *in_dev;

	in_dev = inetdev_get_by_name("eth0");
	local_addr.s_addr = in_dev ? in_dev->ifa_address : 0;
	printf("Welcome to http://%s\n", inet_ntoa(local_addr));
}

static int run_service(const char *service) {
	int ret;

	ret = web_service_add(service);
	if (ret < 0) {
		printf("start the service %s ended in failure\n", service);
		return ret;
	}

	printf("service %s started\n", service);
	return 0;
}

#if 0
static int stop_service(const char *service) {
	int ret;

	ret = web_service_stop(service);
	if (ret < 0) {
		printf("stop the service %s ended in failure\n", service);
		return ret;
	}

	printf("service %s stopped\n", service);
	return 0;
}
#endif


static void * start_server(void *unused) {
	int host, client;
	struct sockaddr_in addr;
	socklen_t addr_len;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (host == -1) {
		perror("servd: socket() failure");
		web_server_started = 0;
		return (void *)-errno;
	}

	if (-1 == bind(host, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("servd: bind() failure");
		web_server_started = 0;
		close(host);
		return (void *)-errno;
	}

	if (-1 == listen(host, 1)) {
		perror("servd: listen() failure");
		web_server_started = 0;
		close(host);
		return (void *)-errno;
	}

	welcome_message();

	while (web_server_started) {
		client = accept(host, (struct sockaddr *) &addr, &addr_len);
		if (client == -1) {
			perror("servd: accept() failure");
			web_server_started = 0;
			close(host);
			return (void *)-errno;
		}
		client_process(client);
	}

	web_server_started = 0;
	close(host);
	return (void *)0;
}

int configure_server(const char *config_file) {
	char *cur, *prev;
	char buff[512];
	FILE *file;

	file = fopen(config_file, "r");
	if (file == NULL) {
		printf("File '%s` doesn't exists\n", config_file);
		return -errno;
	}

	memset(&buff[0], 0, sizeof buff);

	if (fread(buff, 1, sizeof buff - 1, file) < 0) {
		printf("Can't read from file '%s`\n", config_file);
		return -errno;
	}

	prev = buff;
	while (1) {
		cur = strchr(prev, '\n');
		if (cur == NULL) {
			break;
		}
		cur[0] = '\0';
		run_service(prev);
		prev = ++cur;
	}

	fclose(file);

	return 0;
}

int stop_server(void) {
	web_server_started = 0;
	return 0;
}

int main(int argc, char **argv) {
	int opt;

	if (!web_server_started) {
		web_server_task = new_task("servd", start_server, NULL);
		if (web_server_task < 0) {
			printf("Server isn't started, try again\n");
			return web_server_task;
		}
		printf("Server started\n");
	}

	web_server_started = 1;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "c:r:s:Sh"))) {
		switch (opt) {
		case 'c': /*configure - opens configure file and runs services*/
			configure_server(optarg);
			break;
		case 'r': /*run - runs service */
			run_service(optarg);
			break;
#if 0
		case 's': /* stop - stops service */
			stop_service(optarg);
			break;
#endif
		case 'S': /* Stop - stops server */
			if (stop_server() < 0) {
				printf("Can't stop server\n");
			}
			else {
				web_service_remove_all();
				printf("Server is stopped\n");
			}
			break;
		default:
			printf("Unknown option '%c' or unspecified value, see manual\n",
					optopt);
		case 'h':
			printf("Usage: servd [-c file.config] [-s service] [-r service] [-S]\n");
			break;
		}
	}

	return 0;
}
