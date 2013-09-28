/**
 * @file
 * @brief Print network statistic
 *
 * @date 30.12.11
 * @author Alexander Kalmuk
 * @author Alexandr Chernakov
 */

#include <embox/cmd.h>
#include <stdio.h>
#include <net/socket/inet_sock.h>
#include <net/sock.h>
#include <net/l4/udp.h>
#include <net/l4/tcp.h>
#include <net/socket/socket_registry.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/l3/ipv4/ip.h>
#include <fcntl.h>

#define __NS_TEST_SOCKET_CREATE__ /*Undefine it, if testing socket is not nessessary*/

EMBOX_CMD(exec);

static unsigned short get_port(const struct sockaddr_in *sa) {
	return ntohs(sa->sin_port);
}

static const char *socket_state_string(enum sock_state st) {
	switch (st) {
	default:               break;
	case SS_UNCONNECTED:   return "UNCONNECTED";
	case SS_BOUND:         return "BOUND";
	case SS_LISTENING:     return "LISTENING";
	case SS_CONNECTING:    return "CONNECTING";
	case SS_CONNECTED:     return "CONNECTED";
	case SS_ESTABLISHED:   return "ESTABLISHED";
	case SS_DISCONNECTING: return "DISCONNECTING";
	case SS_CLOSED:        return "CLOSED";
	}

	assert(0, "illegal socket state");
	return NULL;
}

static void print_ip_addr(in_addr_t ip) {
	size_t i;
	unsigned int ip_addr[4];
	for (i = 0; i < 4; i++) {
		ip_addr[i] = (ip >> (8 * i)) & 0xFF;
	}

	for (i = 3; i != 0; i--) {
		printf("%d%s", ip_addr[i], ".");
	}
	printf("%d ", ip_addr[0]);
}

static void print_inet_socket_info(const struct sr_external_socket_array_node *sinfo) {
	struct sockaddr_in *ssa_in = (struct sockaddr_in *) &(sinfo->saddr);
	struct sockaddr_in *dsa_in = (struct sockaddr_in *) &(sinfo->daddr);
	printf("State: %s ", socket_state_string(sinfo->socket_connection_state));

	printf("Local IP: ");
	print_ip_addr(ssa_in->sin_addr.s_addr);
	printf("Local port: %d ", get_port(ssa_in));

	printf("Remote IP: ");
	print_ip_addr(dsa_in->sin_addr.s_addr);
	printf("Remote port: %d\n", get_port(dsa_in));
}

static void print_generic_socket_info(const struct sr_external_socket_array_node *sinfo) {
	printf("State: %s ", socket_state_string(sinfo->socket_connection_state));
}

static void print_socket_info(const struct sr_external_socket_array_node *sinfo) {
	switch (sinfo->saddr.sa_family) {
	case AF_INET:
		print_inet_socket_info(sinfo);
		break;
	default:
		print_generic_socket_info(sinfo);
		break;
	}
}

static int exec(int argc, char **argv) {
	size_t i, count;
	int ts;
	struct sr_external_socket_array_node *sock_array;
	struct sockaddr_in saddr;

	#ifdef __NS_TEST_SOCKET_CREATE__
	ts = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /*Just creating a socket for testing to be sure that
	                                                at least one socket exists*/
	saddr.sin_family = AF_INET;
	saddr.sin_port   = htons(1234);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(ts, (struct sockaddr *) &saddr, sizeof(struct sockaddr));
	#endif

	if (!(sock_array = sr_get_all_sockets_array(&count))) {
		#ifdef __NS_TEST_SOCKET_CREATE__
		close(ts);
		#endif
		printf("Can't get sockets array. It might be so caused by lack of memory.");
		return 1;
	}
	printf("Array sockets count: %d\n", count);
	for (i = 0; i < count; i++) {
		print_socket_info(sock_array + i);
	}

	sr_free_all_sockets_array(sock_array);

	#ifdef __NS_TEST_SOCKET_CREATE__
	close(ts);
	#endif

	return 0;
}
