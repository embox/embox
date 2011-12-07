/**
 * @file
 * @brief
 * @date 06.12.11
 * @author Anton Kozlov
 *	- TCP version
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>
#include <getopt.h>
#include <kernel/prom_printf.h>
#include <net/port.h>
#include <err.h>

EMBOX_EXAMPLE(httpd_exec);

#define FILE_BUF_SZ 1024
#define REQ_BUF_SZ 128
#define FILE_READ_CHUNK_SZ 2

static int httpd_exec(int argc, char **argv) {
	int sock, connect_sock;
	//int sock2;
	struct sockaddr_in addr;
	struct sockaddr_in dst;
	int dst_addr_len = 0;

	int fd, bytes_read;
	char file_buf[FILE_BUF_SZ], req_buf[REQ_BUF_SZ];
	char *f = file_buf;


	addr.sin_family = AF_INET;
	addr.sin_port= htons(80);
	addr.port_type = TCP_PORT;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	f += sprintf(file_buf, "HTTP/1.0 200 OK\n\n");

	fd = open("hello.html", 0);

	if (fd < 0) {
	    printf("Cannot open file\n");
	    return -1;
	}

	while ((bytes_read = read(fd, f, FILE_READ_CHUNK_SZ)) > 0) {
	    f += bytes_read;
	}

	while (1) {
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(sock < 0) {
			prom_printf("%s", "can't create socket!");
		}

		if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
			printf("%s","sock can't bind!");
		}

		listen(sock, 1);
		connect_sock = accept(sock, (struct sockaddr *) &dst, &dst_addr_len);

		if (connect_sock < 0) {
			printf("accept fail\n");
			return -1;
		}

		while ((bytes_read = recvfrom(connect_sock, req_buf, 3, 0, NULL, 0)) != 0) {
			if (strncmp(req_buf, "GET", 3) == 0) {
				break;
			}
		}

		sendto(connect_sock, file_buf, f - file_buf, 0, (struct sockaddr *) &dst, sizeof(dst));
		close(connect_sock);

	}

	return 0;
}
