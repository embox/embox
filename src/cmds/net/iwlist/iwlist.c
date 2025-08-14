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
#include <net/cfg80211.h>

#include <lib/libds/array.h>



static inline void print_usage(void) {
	printf("Usage:\n");
	printf("  %s <interface> scanning\n");
}

int main(int argc, char *argv[]) {
	struct in_device *iface;
	struct cfg80211_scan_request req;

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
		struct cfg80211_ssid ssids[32] = {0};
		int i;

		req.ssids = ssids;
		req.n_ssids = 32;

		printf("scanning %s\n", iface->dev->name);
		cfg80211_scan(iface->dev, &req);
		printf("Found %d ssid:\n", req.n_ssids);
		for (i = 0; i < req.n_ssids; i ++) {
			printf("\t%d. %s\n", i, req.ssids[i].ssid);
		}
	}
	return 0;
}
