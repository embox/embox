/**
 * @file
 * @brief
 * We send net packet with two messages (@see example/net/sender.c) from host
 * with OS Linux. Reseiver on host with OS Embox get this messages and print them.
 * Example demonstrate how it work.
 * How it work:
 *   Execute receiver on Embox. Then you can execute sender on Linux to send messages.
 *
 * @date 17.10.11
 * @author Alexander Kalmuk
 * @see example/net/sender.c
 */

#include <stdio.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>
#include <getopt.h>
#include <kernel/prom_printf.h>
#include <err.h>

EMBOX_EXAMPLE(exec);

static int exec(int argc, char **argv) {
	int sock;
	//int sock2;
	struct sockaddr_in addr;
	char buf[1024];
	int bytes_read;

	/* Create UDP socket for getting IP packets
	 * AF_INET - protocol type (IP in this case)
	 * SOCK_DGRAM - socket type (UDP in this case)
	 * @return On success, a file descriptor for the new socket is returned.*/
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	//sock2 = socket(AF_INET, SOCK_DGRAM, 0);
	/* check if file descriptor is positive*/
	if (sock < 0) {
		prom_printf("%s", "can't create socket!");
		return sock;
	}

	/* form address socket assign to*/
	addr.sin_family = AF_INET;
	//addr.sin_port = htons(0x1);
	//addr.port_type = UDP_PORT;
	addr.sin_port= htons(0x1);
	/* we want assign to socket UDP port*/
	//addr.port_type = UDP_PORT;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* assigns the address specified to by addr to the socket referred to
	 * by the file descriptor sock. You can bind only one socket on port of concrete type */
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		printf("%s","sock can't bind!");
		return -1;
	}

	/*if(bind(sock2, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
	     prom_printf("%s","sock2 can't bind!");
	}*/

	/* write data form socket in buffer buf. And then print buffer data */
	while (1) {
		bytes_read = recvfrom(sock, buf, 1024, 0, NULL, NULL);
		buf[bytes_read] = '\0';
		printf("%s",buf);
	}

	return 0;
}
