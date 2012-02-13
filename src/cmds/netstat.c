/**
 * @file
 * @brief Print network statistic
 *
 * @date 30.12.11
 * @author Alexander Kalmuk
 */

#include <embox/cmd.h>
#include <stdio.h>
#include <net/inet_sock.h>
#include <net/sock.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/port.h>

EMBOX_CMD(exec);

static unsigned int ip_addr[4];

static void ip_addr_print(in_addr_t ip) {
	for (int i = 0; i < 4; i++) {
		ip_addr[i] = (ip >> (8 * i)) & 0xFF;
	}

	for (int i = 3; i != 0; i--) {
		printf("%d%s", ip_addr[i], ".");
	}
	printf("%d\n", ip_addr[0]);
}

static void get_proto_info(char **array, int port_nr, int elem_size) {
	size_t i;
	struct sock *sk;
	struct inet_sock *inet;

	for (i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
		if (array[elem_size * i]) {
			sk = (struct sock *) array[elem_size * i];
			inet = inet_sk(sk);
			if (inet->sport == port_nr) {
				ip_addr_print(htonl(inet->daddr));
			}
		}
	}
}

static int exec(int argc, char **argv) {
	size_t port_nr;
	void *udp_hash, *tcp_hash;

	printf("%s", "Proto Port Foreign Address State\n");
	tcp_hash = get_tcp_sockets();
	udp_hash = get_udp_sockets();

	for (port_nr = 0; port_nr < SYSTEM_PORT_MAX_NUMBER; port_nr++) {
		if (socket_port_is_busy(port_nr, 1)) {
			printf("tcp   %d  ", port_nr);
			get_proto_info((char**) tcp_hash, port_nr, sizeof(tcp_sock_t*) / 4);
		} else if (socket_port_is_busy(port_nr, 2)) {
			printf("udp   %d  ", port_nr);
			get_proto_info((char**) udp_hash, port_nr, sizeof(udp_sock_t*) / 4);
		}
	}
	return 0;
}
