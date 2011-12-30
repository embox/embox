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
#include <net/port.h>

EMBOX_CMD(exec);

extern udp_sock_t *udp_hash[CONFIG_MAX_KERNEL_SOCKETS];

static unsigned char ip_addr[7];

static void ip_addr2readable(in_addr_t ip) {
	int i, j;

	for(i = 0; i < 4; i++) {
		ip_addr[i + j] = (ip >> (i*8)) & 0xFF; j++;
		ip_addr[i + j] = '.';
	}
	ip_addr[i + j - 1] = '\0';
}

static int exec(int argc, char **argv) {
	size_t port_nr, i;
	struct sock *sk;
	struct inet_sock *inet;

	printf("%s", "Proto Port Foreign Address State\n");

	for(port_nr = 0; port_nr < SYSTEM_PORT_MAX_NUMBER; port_nr++) {
		if(socket_port_is_busy(port_nr, 1)) {
			printf("tcp %d ", port_nr);
		} else if (socket_port_is_busy(port_nr, 2)) {
			printf("udp %d ", port_nr);
			for(i = 0; i < CONFIG_MAX_KERNEL_SOCKETS; i++) {
				if (udp_hash[i]) {
					sk = (struct sock *) udp_hash[i];
					inet = inet_sk(sk);
					if(inet->sport == port_nr) {
						ip_addr2readable(inet->daddr);
						printf("%s \n", ip_addr);
					}
				}
			}
		}
	}

	return 0;
}
