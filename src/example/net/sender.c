/**
 * @file
 * @brief
 * We send net packet with two messages (@see example/net/sender.c) from host
 * with OS Linux. Reseiver on host with OS Embox get this messages and print them.
 * Example demonstrate how it work.
 * How it work:
 *    Execute receiver on Embox. Then execute sender on Linux to send messages.
 *
 * @date 17.10.11
 * @author Alexander Kalmuk
 * @see example/net/recever.c
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

char msg1[] = "Hello Embox!\n";
char msg2[] = "Bye bye!\n";

int main() {
	int sock;
	struct sockaddr_in addr;

	/* Create UDP socket for getting IP packets
	 * AF_INET - protocol type (IP in this case)
	 * SOCK_DGRAM - socket type (UDP in this case)
	 * @return On success, a file descriptor for the new socket is returned.*/
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}

	/* form address socket assign to */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = htonl((in_addr_t) 0x0A000210);

	/* you can send so */
	sendto(sock, msg1, sizeof(msg1), 0,
			(struct sockaddr *)&addr, sizeof(addr));
	/* initiate a connection on a socket */
	connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	/* Or you can send so. Send call may be used only when the
	   socket is in a connected state */
	send(sock, msg2, sizeof(msg2), 0);

	close(sock);

	return 0;
}
