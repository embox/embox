/**
 * @file
 * @brief
 *
 * @date 29.07.2025
 * @author Anton Bondarev
 */

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <stdio.h>

#include <netinet/in.h>
#include <net/if.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/util/macaddr.h>

#include <lib/libds/array.h>



static inline void print_usage(void) {
	printf("Usage:\n");
	printf("  %s <interface> scanning\n");
}

int main(int argc, char *argv[]) {
	struct in_device *iface;

	iface = NULL;

	if (!strcmp("-h", argv[1]) || !strcmp("--help", argv[1])) {
		print_usage();
		return 0;
	}
	
	if (argc < 3) {
		print_usage();
		return 0;
	}

	iface = inetdev_get_by_name(argv[1]);
	if (!strcmp("scan", argv[2])) {
		printf("scanning %s\n", iface->dev->name);
	}
	return 0;
}
