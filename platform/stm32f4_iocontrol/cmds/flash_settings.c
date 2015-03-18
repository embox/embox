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

#include <drivers/flash/flash_dev.h>

#include "libleddrv.h"
#include "led_names.h"

#include <fs/dvfs.h>

#define FLASHSET_MAGIC 0x5500AAAA
#define FLASHSET_WHAT_NET 0x00000001
#define FLASHSET_WHAT_LED 0x00000002
#define FLASHSET_WHAT_LEDNAMES 0x00000004

struct flashset_settings {
	uint32_t fsn_magic;
	uint32_t fsn_what_mask;
	struct sockaddr fsn_addr;
	struct sockaddr fsn_mask;
	unsigned char fsn_mac[MAX_ADDR_LEN];
	unsigned char fsn_leds_state[LEDDRV_LED_N];
	char fsn_led_names[LEDNAME_N][LEDNAME_MAX + 1];
};
static struct flashset_settings flashset_g_settings;

static inline int flashset_is_stored(struct flashset_settings *fl_settings,
					uint32_t mask) {
	return fl_settings->fsn_what_mask & mask;
}

static inline void flashset_make_store(struct flashset_settings *fl_settings,
					uint32_t mask) {
	fl_settings->fsn_what_mask |= mask;
}

static int flashset_nic_store(const char *nic_name,
				struct flashset_settings *fl_settings) {
        struct ifaddrs *i_ifa, *nic_ifa, *ifa;
	struct in_device *iface_dev;
	int errcode;

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

	memcpy(&fl_settings->fsn_addr, nic_ifa->ifa_addr,
		sizeof(fl_settings->fsn_addr));
	memcpy(&fl_settings->fsn_mask, nic_ifa->ifa_netmask,
		sizeof(fl_settings->fsn_mask));
	memcpy(&fl_settings->fsn_mac, iface_dev->dev->dev_addr,
		sizeof(fl_settings->fsn_mac));
	flashset_make_store(fl_settings, FLASHSET_WHAT_NET);

outerr:
	return errcode;
}

static int flashset_nic_restore(const char *nic_name,
				struct flashset_settings *fl_settings) {
	struct in_device *iface_dev;
	int errcode;

	if (!flashset_is_stored(fl_settings, FLASHSET_WHAT_NET)) {
		return 0;
	}

	iface_dev = inetdev_get_by_name(nic_name);
	if (!iface_dev) {
		errcode = -ESRCH;
		goto outerr;
	}

	if ((errcode = inetdev_set_addr(iface_dev,
		((struct sockaddr_in *) &fl_settings->fsn_addr)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = inetdev_set_mask(iface_dev,
		((struct sockaddr_in *) &fl_settings->fsn_mask)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = netdev_set_macaddr(iface_dev->dev, fl_settings->fsn_mac))) {
		goto outerr;
	}

	errcode = 0;
outerr:
	return errcode;
}

static int flashset_led_store(struct flashset_settings *fl_settings) {
	int errcode;
	if (0 > (errcode = leddrv_getstates(fl_settings->fsn_leds_state))) {
		return errcode;
	}

	flashset_make_store(fl_settings, FLASHSET_WHAT_LED);

	return 0;
}

static int flashset_led_restore(struct flashset_settings *fl_settings) {
	if (!flashset_is_stored(fl_settings, FLASHSET_WHAT_LED)) {
		return 0;
	}
	return leddrv_updatestates(fl_settings->fsn_leds_state);
}

static int flashset_lednames_store(struct flashset_settings *fl_settings) {
	memcpy(fl_settings->fsn_led_names, led_names, sizeof(led_names));
	flashset_make_store(fl_settings, FLASHSET_WHAT_LEDNAMES);
	return 0;
}

static int flashset_lednames_restore(struct flashset_settings *fl_settings) {
	if (flashset_is_stored(fl_settings, FLASHSET_WHAT_LEDNAMES)) {
		memcpy(led_names, fl_settings->fsn_led_names, sizeof(led_names));
	}
	return 0;
}

static int flashset_load(struct flashset_settings *fl_settings) {
	struct file file;
	dvfs_open("flashset", &file);
	int errcode = dvfs_read(&file, (char*) fl_settings, sizeof(*fl_settings));

	if (errcode < 0)
		return errcode;
	if (fl_settings->fsn_magic != FLASHSET_MAGIC)
		return -ENOENT;

	return 0;
}

int main(int argc, char *argv[]) {
	struct flashset_settings *const fl_settings = &flashset_g_settings;
	int errcode;
	struct file file;

	if (0 == strcmp(argv[1], "store")) {
		printf(	"Storing flash settings:\n"
				"Magic: %x\n Mask:  %x\n",
				fl_settings->fsn_magic, fl_settings->fsn_what_mask);
		if (0 > (errcode = flashset_load(fl_settings))) {
			if (errcode == -ENOENT) {
				fl_settings->fsn_magic = FLASHSET_MAGIC;
				fl_settings->fsn_what_mask = 0;
			} else {
				return errcode;
			}
		}

		/* storing requested info */
		for (int i_arg = 2; i_arg < argc; ++i_arg) {
			const char *what = argv[i_arg];

			if (0 == strcmp(what, "net")) {
				errcode = flashset_nic_store("eth0", fl_settings);
			} else if (0 == strcmp(what, "led")) {
				errcode = flashset_led_store(fl_settings);
			} else if (0 == strcmp(what, "led_names")) {
				errcode = flashset_lednames_store(fl_settings);
			} else {
				fprintf(stderr, "unknown store object: %s\n", what);
				errcode = -EINVAL;
			}

			if (0 > errcode) {
				return errcode;
			}
		}

		/* ready to write */
		dvfs_open("flashset", &file);
		errcode = dvfs_write(&file, (char*) fl_settings, sizeof(struct flashset_settings));
		if (errcode < 0)
			return errcode;

	} else if (0 == strcmp(argv[1], "restore")) {

		if (0 > (errcode = flashset_load(fl_settings))) {
			printf("Failed to load settings\n"
				"Magic: %x\n Mask: %x\n",
				fl_settings->fsn_magic, fl_settings->fsn_what_mask);
			return 0;
		}

		/* seems to have valid settings */
		printf("Restoring flash settings:\n"
			"Magic: %x\n Mask: %x\n",
			fl_settings->fsn_magic, fl_settings->fsn_what_mask);
		if (0 > (errcode = flashset_nic_restore("eth0", fl_settings))) {
			return errcode;
		}
		if (0 > (errcode = flashset_led_restore(fl_settings))) {
			return errcode;
		}
		if (0 > (errcode = flashset_lednames_restore(fl_settings))) {
			return errcode;
		}
	}

	return 0;
}

