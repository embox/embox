/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.10.2014
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <ifaddrs.h>

#include <net/inetdevice.h>

#include <drivers/block_dev/flash/flash_dev.h>

#include "libleddrv.h"
#include "led_names.h"

#include <fs/dvfs.h>

#define FLASHSET_MAGIC 0x5500AAAA
#define FLASHSET_WHAT_NET 0x00000001
#define FLASHSET_WHAT_LED 0x00000002
#define FLASHSET_WHAT_LEDNAMES 0x00000004

struct flashset_network_settings {
	struct sockaddr fsn_addr;
	struct sockaddr fsn_mask;
	unsigned char   fsn_mac[MAX_ADDR_LEN];
};

static struct flashset_network_settings fsn_network;
static unsigned char fsn_leds_state[LEDDRV_LED_N];

static int flashset_nic_store(const char *nic_name) {
        struct ifaddrs *i_ifa, *nic_ifa, *ifa;
	struct in_device *iface_dev;
	int errcode;
	struct file file;

	ifa = NULL;
        if (-1 == (errcode = getifaddrs(&ifa))) {
		goto outerr;
        }

	nic_ifa = NULL;
        for (i_ifa = ifa; i_ifa != NULL; i_ifa = i_ifa->ifa_next) {

                if (i_ifa->ifa_addr == NULL ||
			i_ifa->ifa_addr->sa_family != AF_INET) {
			continue;
		}

		if (0 == strcmp(i_ifa->ifa_name, nic_name)) {
			nic_ifa = i_ifa;
			break;
		}
	}
	freeifaddrs(ifa);

	if (!nic_ifa) {
		errcode = -ESRCH;
		goto outerr;
	}

	iface_dev = inetdev_get_by_name(nic_name);
	if (!iface_dev) {
		errcode = -EIO;
		goto outerr;
	}

	memcpy(&fsn_network.fsn_addr, nic_ifa->ifa_addr,
		sizeof(fsn_network.fsn_addr));
	memcpy(&fsn_network.fsn_mask, nic_ifa->ifa_netmask,
		sizeof(fsn_network.fsn_mask));
	memcpy(&fsn_network.fsn_mac, iface_dev->dev->dev_addr,
		sizeof(fsn_network.fsn_mac));

	dvfs_open("nic", &file, DFS_CREAT);
	errcode = dvfs_write(&file, (char*) &fsn_network, sizeof(fsn_network));

outerr:
	return errcode;
}

static int flashset_nic_restore(const char *nic_name) {
	struct in_device *iface_dev;
	int errcode;
	struct file file;

	errcode = dvfs_open("nic", &file, 0);
	if (errcode)
		return errcode;
	errcode = dvfs_read(&file, (char*) &fsn_network, sizeof(fsn_network));
	if (errcode < 0)
		return errcode;

	iface_dev = inetdev_get_by_name(nic_name);
	if (!iface_dev) {
		errcode = -ESRCH;
		goto outerr;
	}

	if ((errcode = inetdev_set_addr(iface_dev,
		((struct sockaddr_in *) &fsn_network.fsn_addr)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = inetdev_set_mask(iface_dev,
		((struct sockaddr_in *) &fsn_network.fsn_mask)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = netdev_set_macaddr(iface_dev->dev, fsn_network.fsn_mac))) {
		goto outerr;
	}

	errcode = 0;
outerr:
	return errcode;
}

static int flashset_led_store(void) {
	int errcode;
	struct file file;
	if (0 > (errcode = leddrv_getstates(fsn_leds_state))) {
		return errcode;
	}

	dvfs_open("led", &file, DFS_CREAT);

	return dvfs_write(&file, (char*) &fsn_leds_state, sizeof(fsn_leds_state));
}

static int flashset_led_restore(void) {
	struct file file;
	int errcode;
	errcode = dvfs_open("led", &file, 0);
	if (errcode)
		return errcode;
	errcode = dvfs_read(&file, (char*) &fsn_leds_state, sizeof(fsn_leds_state));
	if (errcode < 0)
		return errcode;

	return leddrv_updatestates(fsn_leds_state);
}

static int flashset_lednames_store(void) {
	struct file file;
	dvfs_open("led_names", &file, DFS_CREAT);
	return dvfs_write(&file, (char*) &led_names, sizeof(led_names));
}

static int flashset_lednames_restore(void) {
	struct file file;
	if (dvfs_open("led_names", &file, 0))
		return -1;
	return 0 > dvfs_read(&file, (char*) &led_names, sizeof(led_names));;
}

int main(int argc, char *argv[]) {
	int errcode;

	if (0 == strcmp(argv[1], "store")) {
		printf(	"Storing flash settings\n");
		/* storing requested info */
		for (int i_arg = 2; i_arg < argc; ++i_arg) {
			const char *what = argv[i_arg];

			if (0 == strcmp(what, "net")) {
				errcode = flashset_nic_store("eth0");
			} else if (0 == strcmp(what, "led")) {
				errcode = flashset_led_store();
			} else if (0 == strcmp(what, "led_names")) {
				errcode = flashset_lednames_store();
			} else {
				fprintf(stderr, "unknown store object: %s\n", what);
				errcode = -EINVAL;
			}

			if (0 > errcode) {
				return errcode;
			}
		}
	} else if (0 == strcmp(argv[1], "restore")) {
		/* seems to have valid settings */
		printf("Restoring flash settings\n");

		printf("  net       [%s]\n"
		       "  led       [%s]\n"
		       "  led_names [%s]\n",
		flashset_nic_restore("eth0") ? "fail" : " ok ",
		flashset_led_restore()       ? "fail" : " ok ",
		flashset_lednames_restore()  ? "fail" : " ok ");
	}

	return 0;
}

