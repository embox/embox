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
	printf("  %s <interface> essid Wifi2Home key s:ABCDE12345\n");
}

int main(int argc, char *argv[]) {
	int i;
	struct in_device *iface = NULL;
	uint8_t essid_name[IEEE80211_MAX_SSID_LEN] = {0};
	uint8_t key_str[32] = {0};
	struct cfg80211_connect_params sme;


	for (i = 1; i < argc; ++i) {
		if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
			print_usage();
			return 0;
		}

		if (!strcmp("essid", argv[i])) {
			i++;
			strncat((void *)essid_name, argv[i], sizeof(essid_name) - 1);
			continue;

		}

		if (!strcmp("key", argv[i])) {
			i++;
			strncat((void *)key_str, &(argv[i][2]), sizeof(key_str) - 1); /* 's:xxxx' */
			continue;

		}
	}


	iface = inetdev_get_by_name(argv[1]);

	sme.ssid = essid_name;
	sme.key  = key_str;
	sme.ssid_len = strlen((void *)essid_name);

	printf("setup iface %s essid %s key %s\n", iface->dev->name, essid_name, key_str);

	cfg80211_connect(iface->dev, &sme);


	return 0;
}
