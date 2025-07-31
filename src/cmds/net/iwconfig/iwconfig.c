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
	printf("  %s <interface> essid Wifi2Home key s:ABCDE12345\n");
}

int main(int argc, char *argv[]) {
	int i;
	struct in_device *iface = NULL;
	char *essid_name = NULL;
	char *key_str = NULL;


	for (i = 1; i < argc; ++i) {
		if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
			print_usage();
			return 0;
		}

		if (!strcmp("essid", argv[i])) {
			i++;
			essid_name = argv[i];
			continue;

		}

		if (!strcmp("key", argv[i])) {
			i++;
			key_str = &(argv[i][2]); /* 's:xxxx' */
			continue;

		}
	}

	if ((key_str == NULL) || (essid_name == NULL)) {
		print_usage();
		return 0;
	}

	iface = inetdev_get_by_name(argv[1]);

	printf("setup iface %s essid %s key\n", iface->dev->name, essid_name, key_str);


	return 0;
}
