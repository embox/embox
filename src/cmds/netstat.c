/**
 * @file
 * @brief Print network statistic
 * @date 30.12.11
 * @author Alexander Kalmuk
 * @author Alexandr Chernakov
 */

#include <embox/cmd.h>
#include <stdio.h>
#include <net/inet_sock.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/socket_registry.h>
#include <net/socket.h>
#include <net/ip.h>

#define __NS_TEST_SOCKET_CREATE__ /*Undefine it, if testing socket is not nessessary*/

EMBOX_CMD(exec);

static unsigned short get_port (struct sockaddr_in * sa) {
	return ntohs(sa->sin_port);
}

static void print_socket_state (enum socket_connection_state_t st) {
	switch (st) {
	case UNCONNECTED:
		printf ("UNCONNECTED ");
		break;
	case CONNECTED:
		printf ("CONNECTED ");
		break;
	case CONNECTING:
		printf ("CONNECTING ");
		break;
	case CLOSED:
		printf ("CLOSED ");
		break;
	case LISTENING:
		printf ("LISTENING ");
		break;
	case BOUND:
		printf ("BOUND ");
		break;
	case ESTABLISHED:
		printf ("ESTABLISHED ");
		break;
	case DISCONNECTING:
		printf ("DISCONNECTING ");
		break;
	default:
		printf ("UNKNOWN STATE ");
		break;
	}
}

static void print_ip_addr(in_addr_t ip) {
	int i;
	unsigned int ip_addr[4];
	for (i = 0; i < 4; i++) {
		ip_addr[i] = (ip >> (8 * i)) & 0xFF;
	}

	for (i = 3; i != 0; i--) {
		printf("%d%s", ip_addr[i], ".");
	}
	printf("%d ", ip_addr[0]);
}

static void print_inet_socket_info (struct ns_external_socket_array_node * sinfo) {
	struct sockaddr_in * ssa_in = (struct sockaddr_in *) &(sinfo->saddr);
	struct sockaddr_in * dsa_in = (struct sockaddr_in *) &(sinfo->daddr);
	printf ("State: ");
	print_socket_state (sinfo->socket_connection_state);

	printf ("Local IP: ");
	print_ip_addr (ssa_in->sin_addr.s_addr);
	printf ("Local port: %d ", get_port(ssa_in));

	printf ("Remote IP: ");
	print_ip_addr (dsa_in->sin_addr.s_addr);
	printf ("Remote port: %d\n", get_port(dsa_in));
}

static void print_generic_socket_info (struct ns_external_socket_array_node * sinfo) {
	printf ("State: ");
	print_socket_state (sinfo->socket_connection_state);
}

static void print_socket_info (struct ns_external_socket_array_node * sinfo) {
	switch (sinfo->saddr.sa_family) {
	case AF_INET:
		break;
		print_inet_socket_info (sinfo);
	default:
		break;
		print_generic_socket_info(sinfo);
	}
}

int exec (int argc, char ** argv) {
	int i, count;
	int ts;
	struct ns_external_socket_array_node * sock_array;
	struct sockaddr_in saddr;

	#ifdef __NS_TEST_SOCKET_CREATE__
	ts = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /*Just creating a socket for testing to be sure that
	                                                at least one socket exists*/
	saddr.sin_family = AF_INET;
	saddr.sin_port= htons(1234);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind (ts, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	#endif

	if (!(sock_array = get_all_sockets_array (&count))) {
		free_all_sockets_array(sock_array);
		#ifdef __NS_TEST_SOCKET_CREATE__
		close(ts);
		#endif
	}
	printf ("Array sockets count: %d\n", count);
	for (i = 0; i < count; i++) {
		print_socket_info(sock_array + i);
	}

	free_all_sockets_array(sock_array);

	#ifdef __NS_TEST_SOCKET_CREATE__
	close(ts);
	#endif

	return 0;
}
