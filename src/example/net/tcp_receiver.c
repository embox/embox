/**
 * @file
 * @brief
 *
 * @date 30.10.11
 * @author Alexander Kalmuk
 *	- UDP version
 * @author Anton Kozlov
 *	- TCP version
 */

#include <stdio.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>
#include <getopt.h>
#include <kernel/prom_printf.h>
#include <net/port.h>
#include <err.h>

EMBOX_EXAMPLE(exec);

static int exec(int argc, char **argv) {
	int sock, connect_sock;
	//int sock2;
	struct sockaddr_in addr;
	struct sockaddr_in dst;
	int dst_addr_len = 0;
	char buf[1024];
	int bytes_read;

	/* Create UDP socket for getting IP packets
	 * AF_INET - protocol type (IP in this case)
	 * SOCK_DGRAM - socket type (UDP in this case)
	 * @return On success, a file descriptor for the new socket is returned.*/
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//sock2 = socket(AF_INET, SOCK_DGRAM, 0);
	/* check if file descriptor is positive*/
	if(sock < 0) {
		prom_printf("%s", "can't create socket!");
	}

	/* form address socket assign to*/
	addr.sin_family = AF_INET;
	//addr.sin_port = htons(0x1);
	//addr.port_type = UDP_PORT;
	addr.sin_port= htons(22);
	/* we want assign to socket UDP port*/
	addr.port_type = TCP_PORT;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* assigns the address specified to by addr to the socket referred to
	 * by the file descriptor sock. You can bind only one socket on port of concrete type */
	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		printf("%s","sock can't bind!");
	}

	listen(sock, 1);

	connect_sock = accept(sock, (struct sockaddr *) &dst, &dst_addr_len);

	if (connect_sock < 0) {
		return -1;
	}

	/* read from sock, print, echo back to sock */
	while (1) {
		if (0 >= (bytes_read = recvfrom(connect_sock, buf, 1024, 0, NULL, NULL))) {
			continue;
		}
		buf[bytes_read] = '\0';
		if (0 == strncmp(buf, "quit", 4)) {
			break;
		}
		printf("%s",buf);
		sendto(connect_sock, buf, bytes_read, 0, (struct sockaddr *) &dst, sizeof(dst));
	}

	close(connect_sock);

	return 0;
}
