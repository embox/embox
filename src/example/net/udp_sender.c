/**
 * @brief Example demonstrates ICMP message handling.
 * We send packet on unreachable port and expect to receive UNREACHABLE PORT error,
 * then we will handle this error.
 * @date 22.12.11
 * @Alexander Kalmuk
 */

#include <stdio.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>
#include <getopt.h>
#include <kernel/prom_printf.h>
#include <net/port.h>
#include <net/icmp.h>
#include <err.h>

EMBOX_EXAMPLE(exec);

char msg1[] = "Hello Linux!\n";

static int exec(int argc, char **argv) {
    int sock;
    struct sockaddr_in addr;
    int err;
    //int bytes_read;
    //char buf[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    /* form address socket assign to */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0xFFFF);
    addr.sin_addr.s_addr = htonl((in_addr_t) 0x0A00020F);

    /* you can send so */
    sendto(sock, msg1, sizeof(msg1), 0,
           (struct sockaddr *)&addr, sizeof(addr));

    err = check_icmp_err(sock);
    if((err & 0x000000FF) == ICMP_DEST_UNREACH) {
		if((err & 0x0000FF00) >> 8 == ICMP_PORT_UNREACH) {
			printf("\nUnreacheble port %d\n", addr.sin_port);
		}
    }

    close(sock);

    return 0;
}
