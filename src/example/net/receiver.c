/**
 *	//TODO commen it
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <net/ip.h>
#include <net/socket.h>
#include <framework/example/self.h>
#include <getopt.h>
#include <kernel/prom_printf.h>

EMBOX_EXAMPLE(exec);

static int exec(int argc, char **argv) {
    int sock;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
    	prom_printf("%s", "sfaf");
    }

    prom_printf("%s", "UDPFFNGNASKG");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        prom_printf("%s","bind");
    }

    while (1) {
        bytes_read = recvfrom(sock, buf, 1024, 0, NULL, NULL);
        buf[bytes_read] = '\0';
        printf("%s",buf);
    }

    return 0;
}
