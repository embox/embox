/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Vita Loginova
 */

#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <net/ip.h>
#include <net/socket.h>
#include <net/inetdevice.h>
#include <cmd/web_server.h>
#include <cmd/servd.h>
#include <embox/web_service.h>
#include <embox/cmd.h>
#include <kernel/task.h>

EMBOX_CMD(servd);

int web_server_started = 0;
int web_server_task;

static void print_usage(void) {
	printf("Usage: servd [-c file.config] [-s service] [-r service] [-S]\n");
}

static void welcome_message(void) {
	/* FIXME cheat to get local ip */
	struct in_addr localAddr;
	struct in_device *in_dev = inet_dev_find_by_name("eth0");
	localAddr.s_addr = in_dev->ifa_address;
	printf("Welcome to http://%s\n", inet_ntoa(localAddr));
}

static void *start_server(void* args) {
	int res, host;
	socklen_t addr_len;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (host < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		web_server_started = -1;
		return (void*) host;
	}

	res = bind(host, (struct sockaddr *) &addr, sizeof(addr));
	if (res < 0) {
		printf("Error.. bind() failed. errno=%d\n", errno);
		web_server_started = -1;
		close(host);
		return (void*) res;
	}

	res = listen(host, 1);
	if (res < 0) {
		printf("Error.. listen() failed. errno=%d\n", errno);
		web_server_started = -1;
		close(host);
		return (void*) res;
	}

	welcome_message();

	while (web_server_started) {
		res = accept(host, (struct sockaddr *) &addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			printf("Error.. accept() failed. errno=%d\n", errno);
			close(host);
			web_server_started = 0;
			return (void*) res;
		}
		client_process(res);
	}

	close(host);
	return (void*) 0;
}

int run_service(char * service) {
	if (0 <= web_service_add(service)) {
		printf("service %s started\n", service);
		return 0;
	}

	printf("service %s doesn't exists, is started yet or starting error\n",
			service);
	return -1;
}

/*int stop_service(char * service) {
	if (0 <= web_service_stop(service)) {
		printf("service %s stopped\n", service);
		return 0;
	}

	printf("service %s isn't started\n", service);
	return -1;
}*/

int stop_server(void) {
	int sock, res;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(inet_addr("10.0.2.16"));

	res = connect(sock, (struct sockaddr *) &addr, sizeof(addr));
	if (res < 0) {
		close(sock);
		return -1;
	}

	close(sock);
	return 0;
}

static int servd(int argc, char **argv) {
	char opt;
	char *cur, *prev;
	char buff[512];
	FILE *file;

	if (!web_server_started) {
		web_server_started =
				(web_server_task = new_task(start_server, NULL)) >= 0 ? 1 : 0;
	}

	if (web_server_started) {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "c:r:s:Sh"))) {
			switch (opt) {
			case 'c': /*configure - opens configure file and runs services*/
				file = fopen(optarg, "r");
				if (file == NULL) {
					printf("File doesn't exists\n");
					break;
				}
				fread(buff, 1, sizeof buff, file);
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
				break;
			case 'r': /*run - runs service*/
				run_service(optarg);
				break;
			/*case 's': stop - stops service
				stop_service(optarg);*/
				break;
			case 'S': /*Stop - stops server*/
				web_server_started = 0;
				if (0 <= stop_server()) {
					web_service_remove_all();
					printf("Server is stopped\n");
				} else {
					web_server_started = 1;
					printf("Can't stop server\n");
				}
				break;
			case 'h':
				print_usage();
				break;
			default:
				printf("Unknown option '%c' or unspecified value, see manual\n",
						optopt);
				print_usage();
				break;
			}
		}
	} else {
		printf("Server isn't started, try again\n");
	}

#if 0
	if (0 != thread_create(&thr, THREAD_FLAG_DETACHED, start_server, NULL)) {
		return -1;
	}
#endif

	return 0;
}

