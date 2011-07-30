/**
 * @file
 * @brief Example demonstrates the work of sockets and loopback interface
 *
 * @date 29.03.11
 * @author Dmitry Zubarevich
 */

#include <stdio.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>

/**This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

static int run(int argc, char **argv) {
	/* msg_send - test message for send */
	char msg_send[] = "'Test_message'";
	/* sock - identifier of socket */
	int sock;
	/* msg_size - size of test message */
	ssize_t msg_size = sizeof(msg_send);
	/* addr_size - size of address */
	socklen_t addr_size;
	/* msg_recv - receive message */
	char msg_recv[msg_size];
	/* addr - address of local host */
	struct sockaddr_in addr;

	/** creation of socket
	 * AF_INET - identifier of Internet IP Protocol family
	 * SOCK_DGRAM - datagram socket
	 * 0 - Normally only a single protocol exists to support a
	 * particular socket type within a given protocol family,
	 * in which case protocol can be specified as 0.
	 *
	 * function returns 0 if creation is success
	 * */
	if (0 > (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
		/* if creation was failed report about it and exit from test */
		TRACE("\nCan't create socket");
		return 0;
	} else {
		/* if creation was success print message about it */
		TRACE("\nSocket was created");
	}

	/* define of address to send and receive of message */
	/** indication of protocol family
	 * (AF_INET - identifier of Internet IP Protocol family)*/
	addr.sin_family = AF_INET;
	/* indication of port (666 - identifier of port) */
	addr.sin_port = htons(666);
	/** indication of ip address
	 * (INADDR_LOOPBACK - constant appropriated to ip address 127.0.0.1 - local host)*/
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	/* get size of address */
	addr_size = sizeof(addr);

	/** transmitting of test message
	 * sock - identifier of socket
	 * msg_send - transmitted message
	 * msg_size - size of transmitted message
	 * 0 - combination of bit flags
	 * addr - address of receiver
	 * addr_size - size of address
	 *
	 * function returns 0 if sending is success
	 * */
	if (0 > (sendto(sock, msg_send, msg_size, 0, (struct sockaddr *) &addr, addr_size))) {
		/* if transmission was failed report about it and exit from test */
		TRACE("\nCan't send message");
		return 0;
	} else {
		/* if sending was success print sent message */
		TRACE("\nIt was sent message: %s", msg_send);
	}

	/** receiving of test message
	 * sock - identifier of socket
	 * msg_recv - buffer for received message
	 * 0 - combination of bit flags
	 * addr - address of sender
	 * addr_size - size of address
	 *
	 * function returns 0 if receiving is success
	 * */
	if (0 > (recvfrom(sock,
			&msg_recv,
			msg_size, 0,
			(struct sockaddr *) &addr,
			&addr_size))) {
		/* if receiving was failed report about it and exit from test */
		TRACE("\nCan't receive message");
		return 0;
	} else {
		/* if receiving was success print received message */
		TRACE("\nIt was received message: %s", msg_recv);
	}

	/** close of socket
	 * sock - identifier of socket
	 *
	 * function returns 0 if closing is success
	 * */
	if (0 > close(sock)) {
		/* if close was failed report about it and exit from test */
		TRACE("\nIt was bad close of socket");
		return 0;
	} else {
		/* if creation was success print message about it */
		TRACE("\nSocket was closed");
	}

	return 0;
}
