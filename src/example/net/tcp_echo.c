/**
 * @file
 * @brief
 *
 * @date 03.04.12
 * @author Ilia Vaprol
 */

#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <kernel/printk.h>
#include <net/l3/ipv4/ip.h>
#include <sys/socket.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(exec);

#define LISTENING_PORT 19

static int exec(int argc, char **argv) {
	int sock, client, res, bytes_read;
	struct sockaddr_in addr;
	struct sockaddr_in dst;
	int dst_addr_len = 0;
	char buff[1024];

	printk("Hello, I'm tcp_echo at %d port. I will send all messages back.\n",
			LISTENING_PORT);


	res = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (res < 0) {
		printk("can't create socket %d!\n", res);
		return res;
	}
	sock = res;

	addr.sin_family = AF_INET;
	addr.sin_port= htons(LISTENING_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (res < 0) {
		printk("error at bind() %d!\n", res);
		return res;
	}

	res = listen(sock, 1);
	if (res < 0) {
		printk("error at listen() %d!\n", res);
		return res;
	}

	res = accept(sock, (struct sockaddr *)&dst, &dst_addr_len);
	if (res < 0) {
		printk("error at accept() %d\n", res);
		return res;
	}
	client = res;

	printk("client from %s:%d was connected\n",
			inet_ntoa(dst.sin_addr), ntohs(dst.sin_port));

	/* read from sock, print */
	while (1) {
		dst_addr_len = sizeof dst;
		bytes_read = recvfrom(client, buff, sizeof buff, 0, (struct sockaddr *)&dst,
				&dst_addr_len);
		if (bytes_read < 0) {
			break;
		}
		buff[bytes_read] = '\0';
		if (strncmp(buff, "quit", 4) == 0) {
			printk("exit\n");
			break;
		}
		printk("echo: '%s'\n", buff);
		if (sendto(client, buff, bytes_read, 0, (struct sockaddr *)&dst,
				dst_addr_len) < 0) {
			break;
		}
	}

	close(client);
	close(sock);

	printk("Bye bye!\n");

	return 0;
}
