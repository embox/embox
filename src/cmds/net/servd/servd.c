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
		return (void*) host;
	}

	res = bind(host, (struct sockaddr *) &addr, sizeof(addr));
	if (res < 0) {
		printf("Error.. bind() failed. errno=%d\n", errno);
		return (void*) res;
	}

	res = listen(host, 1);
	if (res < 0) {
		printf("Error.. listen() failed. errno=%d\n", errno);
		return (void*) res;
	}

	welcome_message();

	while (1) {
		res = accept(host, (struct sockaddr *) &addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			printf("Error.. accept() failed. errno=%d\n", errno);
			close(host);
			return (void*) res;
		}
		client_process(res, addr, addr_len);
	}

	close(host);
	return (void*) 0;
}

static int web_server_started = -1;

int srvcrun(char * service) {
	if (0 <= web_service_start(service)) {
		printf("service %s started\n", service);
		return 0;
	}

	printf("service %s doesn't exists, is started yet or starting error\n", service);
	return -1;
}

int srvcstop(char * service) {
	if (0 <= web_service_stop(service)) {
		printf("service %s stopped\n", service);
		return 0;
	}

	printf("service %s isn't started\n", service);
	return -1;
}

static int servd(int argc, char **argv) {
	int i_opt;
	char opt;
	char *cur, *prev;
	char buff[512];
	FILE *file;

	if (0 > web_server_started) {
		web_server_started = new_task(start_server, NULL);
	}

	if (0 <= web_server_started) {
		getopt_init();
		for (i_opt = 0; i_opt < argc - 1; i_opt++) {
			opt = getopt(argc, argv, "c:r:s:S");
			switch (opt) {
			case 'c':
				file = fopen(optarg, "r");
				if (file == NULL){
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
					srvcrun(prev);
					prev = ++cur;
				}
				fclose(file);
				break;
			case 'r':
				srvcrun(optarg);
				break;
			case 's':
				srvcstop(optarg);
				break;
			case 'S':
				break;
			}
		}
	} else {
		printf("Service isn't started");
	}

#if 0
	if (0 != thread_create(&thr, THREAD_FLAG_DETACHED, start_server, NULL)) {
		return -1;
	}
#endif

	return 0;
}
