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
#include <net/ip.h>
#include <sys/socket.h>
#include <net/inetdevice.h>
#include <cmd/web_server.h>
#include <cmd/servd.h>
#include <embox/web_service.h>
#include <embox/cmd.h>
#include <kernel/task.h>
#include <errno.h>
#include <unistd.h>

EMBOX_CMD(servd);

static int web_server_started = 0;
static int web_server_task;

static void welcome_message(void) {
	/* FIXME cheat to get local ip */
	struct in_addr local_addr;
	struct in_device *in_dev;

	in_dev = inet_dev_find_by_name("eth0");
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

static int stop_service(const char *service) {
#if 0
	int ret;

	ret = web_service_stop(service);
	if (ret < 0) {
		printf("stop the service %s ended in failure\n", service);
		return ret;
	}

	printf("service %s stopped\n", service);
#endif
	return 0;
}


static void * start_server(void *unused) {
	int ret, host, client;
	struct sockaddr_in addr;
	socklen_t addr_len;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listen socket */
	host = ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ret < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		web_server_started = 0;
		return (void *)ret;
	}

	ret = bind(host, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		printf("Error.. bind() failed. errno=%d\n", errno);
		web_server_started = 0;
		close(host);
		return (void *)ret;
	}

	ret = listen(host, 1);
	if (ret < 0) {
		printf("Error.. listen() failed. errno=%d\n", errno);
		web_server_started = 0;
		close(host);
		return (void *)ret;
	}

	welcome_message();

	while (web_server_started) {
		client = ret = accept(host, (struct sockaddr *) &addr, &addr_len);
		if (ret < 0) {
			printf("Error.. accept() failed. errno=%d\n", errno);
			web_server_started = 0;
			close(host);
			return (void *)ret;
		}
		client_process(client);
	}

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
		return -1;
	}

	memset(&buff[0], 0, sizeof buff);

	if (fread(buff, 1, sizeof buff - 1, file) < 0) {
		printf("Can't read from file '%s`\n", config_file);
		return -1;
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

static int servd(int argc, char **argv) {
	int opt;

	if (!web_server_started) {
		web_server_task = new_task(start_server, NULL);
		if (web_server_task < 0) {
			printf("Server isn't started, try again\n");
			return 0;
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
		case 's': /* stop - stops service */
			stop_service(optarg);
			break;
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
