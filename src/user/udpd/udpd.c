/**
 * \file udpd.c
 *
 * \date 08.05.09
 * \author sikmir
 */
#include "shell_command.h"
#include "string.h"
#include "misc.h"
#include "net/net.h"
#include "inet/netinet/in.h"
#include "net/socket.h"

#define COMMAND_NAME     "udpd"
#define COMMAND_DESC_MSG "test udp socket"
#define HELP_MSG         "Usage: udpd [-h] [-a addr] [-p port]"
static const char *man_page =
	#include "udpd_help.inc"
	;

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static char available_keys[] = {
	'a', 'p', 'h'
};

static int exec(int argsc, char **argsv) {
	int nextOption;
	unsigned char addr[4];
	unsigned long ip_addr = INADDR_ANY;
	int port = 666;

        getopt_init();
        do {
                nextOption = getopt(argsc, argsv, "a:p:h");
                switch(nextOption) {
                case 'h':
                        show_help();
                        return 0;
                case 'a':
            		if (NULL == ipaddr_scan(optarg, addr)) {
                                LOG_ERROR("wrong ip addr format (%s)\n", optarg);
                                show_help();
                                return -1;
                        } else {
                                ip_addr = inet_addr(addr);
                        }
                        break;
                case 'p':
            		if (1 != sscanf(optarg, "%d", &port)) {
                                LOG_ERROR("enter validly port '-p'\n");
                                show_help();
                                return -1;
                        }
                        break;
                case -1:
                        break;
                default:
                        return 0;
                }
        } while(-1 != nextOption);

	int sock, length, n;
	struct sockaddr_in server;
	char buf[64];
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		LOG_ERROR("opening socket\n");
		return -1;
	}
	length = sizeof(server);
	memset(&server, 0, length);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=ip_addr;
	server.sin_port=htons(port);

	printf("ip=0x%X, port=%d\n", ip_addr, port);
	if(bind(sock, (struct sockaddr *)&server, length)) {
		LOG_ERROR("binding socket\n");
		return -1;
	}
	while (1) {
		n = recv(sock, buf, sizeof(buf), 0);
		if(n < 0) {
			LOG_ERROR("can't recv\n");
			continue;
		}
		printf("buf=%s\n", buf);
		n = send(sock, buf, sizeof(buf), 0);
		if(n < 0) {
			LOG_ERROR("can't send\n");
		}
	}
	close(sock);
	return 0;
}
