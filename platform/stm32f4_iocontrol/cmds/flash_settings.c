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

#define FLASHSET_BLOCK 0

extern const struct flash_dev stm32_flash;

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

static inline int flashset_is_stored(struct flashset_settings *fl_settings, uint32_t mask) {
	return fl_settings->fsn_what_mask & mask;
}

static inline void flashset_make_store(struct flashset_settings *fl_settings, uint32_t mask) {
	fl_settings->fsn_what_mask |= mask;
}

static int flashset_nic_store(const char *nic_name, struct flashset_settings *fl_settings) {
        struct ifaddrs *i_ifa, *nic_ifa, *ifa;
	struct in_device *iface_dev;
	int errcode;

	ifa = NULL;
        if (-1 == (errcode = getifaddrs(&ifa))) {
		goto outerr;
        }

	nic_ifa = NULL;
        for (i_ifa = ifa; i_ifa != NULL; i_ifa = i_ifa->ifa_next) {

                if (i_ifa->ifa_addr == NULL || i_ifa->ifa_addr->sa_family != AF_INET) {
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

	memcpy(&fl_settings->fsn_addr, nic_ifa->ifa_addr, sizeof(fl_settings->fsn_addr));
	memcpy(&fl_settings->fsn_mask, nic_ifa->ifa_netmask, sizeof(fl_settings->fsn_mask));
	memcpy(&fl_settings->fsn_mac, iface_dev->dev->dev_addr, sizeof(fl_settings->fsn_mac));
	flashset_make_store(fl_settings, FLASHSET_WHAT_NET);

outerr:
	return errcode;
}

static int flashset_nic_restore(const char *nic_name, struct flashset_settings *fl_settings) {
	struct in_device *iface_dev;
	int errcode;

	if (!flashset_is_stored(fl_settings, FLASHSET_WHAT_NET)) {
		return -ESRCH;
	}

	iface_dev = inetdev_get_by_name(nic_name);
	if (!iface_dev) {
		errcode = -ESRCH;
		goto outerr;
	}

	if ((errcode = inetdev_set_addr(iface_dev, ((struct sockaddr_in *) &fl_settings->fsn_addr)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = inetdev_set_mask(iface_dev, ((struct sockaddr_in *) &fl_settings->fsn_mask)->sin_addr.s_addr))) {
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
		return -ESRCH;
	}
	return leddrv_updatestates(fl_settings->fsn_leds_state);
}

static int flashset_lednames_store(struct flashset_settings *fl_settings) {
	memcpy(fl_settings->fsn_led_names, led_names, sizeof(led_names));
	flashset_make_store(fl_settings, FLASHSET_WHAT_LEDNAMES);
	return 0;
}

static int flashset_lednames_restore(struct flashset_settings *fl_settings) {
	if (!flashset_is_stored(fl_settings, FLASHSET_WHAT_LEDNAMES)) {
		return -ESRCH;
	}
	memcpy(led_names, fl_settings->fsn_led_names, sizeof(led_names));
	return 0;
}

static int flashset_load(struct flash_dev *flashdev, struct flashset_settings *fl_settings) {
	int errcode;

	if (0 > (errcode = flash_read(flashdev, FLASHSET_BLOCK, fl_settings, sizeof(struct flashset_settings)))) {
		return errcode;
	}
	if (fl_settings->fsn_magic != FLASHSET_MAGIC) {
		return -ENOENT;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	struct flash_dev *flashdev = (struct flash_dev *) &stm32_flash;
	struct flashset_settings *fl_settings = &flashset_g_settings;
	int errcode;

	chdir("/conf");

	if (0 == strcmp(argv[1], "store")) {

		if (0 > (errcode = flashset_load(flashdev, fl_settings))) {
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
		if (0 > (errcode = flash_erase(flashdev, FLASHSET_BLOCK))) {
			return errcode;
		}
		if (0 > (errcode = flash_write(flashdev, FLASHSET_BLOCK, fl_settings, sizeof(struct flashset_settings)))) {
			return errcode;
		}

	} else if (0 == strcmp(argv[1], "restore")) {

		if (0 > (errcode = flashset_load(flashdev, fl_settings))) {
			return 0;
		}

		fprintf(stderr,
			        "  net       [%s]\n"
			        "  led       [%s]\n"
			        "  led_names [%s]\n",
			flashset_nic_restore("eth0", fl_settings) ? "fail" : " ok ",
			flashset_led_restore(fl_settings)       ? "fail" : " ok ",
			flashset_lednames_restore(fl_settings)  ? "fail" : " ok ");
	}

	return 0;
}

