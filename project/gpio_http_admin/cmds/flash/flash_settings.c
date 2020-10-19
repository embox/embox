/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.10.2014
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <ifaddrs.h>
#include <net/inetdevice.h>
#include <drivers/flash/flash.h>

#include <led_driver_lib.h>

#define NIC_FILENAME  "/conf/nic"
#define LED_FILENAME  "/conf/led"

#define MAX_LEDS 32

#define DPRINTF(fmt, ...) \
	fprintf(stderr, "flash_settings: %s: " fmt, __func__, ##__VA_ARGS__)

struct flashset_network_settings {
	struct sockaddr fsn_addr;
	struct sockaddr fsn_mask;
	unsigned char   fsn_mac[MAX_ADDR_LEN];
};

static struct flashset_network_settings fsn_network;
static unsigned char fsn_leds_state[MAX_LEDS];

static int flashset_nic_store(const char *nic_name) {
		struct ifaddrs *i_ifa, *nic_ifa, *ifa;
	struct in_device *iface_dev;
	int errcode, fd;

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

	fd = open(NIC_FILENAME, O_CREAT | O_RDWR);

	errcode = write(fd, (char*) &fsn_network, sizeof(fsn_network));
	close(fd);
	if (errcode < 0) {
		goto outerr;
	}

	return 0;
outerr:
	DPRINTF("failed\n");
	return errcode;
}

static int flashset_nic_restore(const char *nic_name) {
	struct in_device *iface_dev;
	int errcode, fd;

	fd = open(NIC_FILENAME, 0);
	if (fd < 0) {
		DPRINTF("failed\n");
		return fd;
	}

	errcode = read(fd, (char*) &fsn_network, sizeof(fsn_network));
	if (errcode < 0)
		goto outerr;

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

	close(fd);
	return 0;
outerr:
	DPRINTF("failed\n");
	close(fd);
	return errcode;
}

static int flashset_led_store(void) {
	int errcode, fd;

	led_driver_get_states(fsn_leds_state, MAX_LEDS);

	fd = open(LED_FILENAME, O_CREAT | O_RDWR);
	if (fd < 0) {
		DPRINTF("Error opening led\n");
		return -1;
	}
	errcode = write(fd, (char*) &fsn_leds_state, sizeof(fsn_leds_state));
	close(fd);
	if (errcode < 0) {
		DPRINTF("Error writing led\n");
	}

	return errcode;
}

static int flashset_led_restore(void) {
	int errcode, fd;
	fd = open(LED_FILENAME, 0);
	if (fd < 0) {
		DPRINTF("Error opening led\n");
		return -1;
	}
	errcode = read(fd, (char*) &fsn_leds_state, sizeof(fsn_leds_state));
	close(fd);

	if (errcode < 0) {
		DPRINTF("Error reading led\n");
		return errcode;
	}

	return led_driver_set_states(fsn_leds_state, MAX_LEDS);
}

int main(int argc, char *argv[]) {
	int errcode;

	if (0 == strcmp(argv[1], "store")) {
		fprintf(stderr, "Storing flash settings\n");
		/* storing requested info */
		for (int i_arg = 2; i_arg < argc; ++i_arg) {
			const char *what = argv[i_arg];

			DPRINTF("Storing setting \"%s\"\n", what);

			if (0 == strcmp(what, "net")) {
				errcode = flashset_nic_store("eth0");
			} else if (0 == strcmp(what, "led")) {
				errcode = flashset_led_store();
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
		fprintf(stderr, "Restoring flash settings\n");

		fprintf(stderr,
			"  net       [%s]\n"
			"  led       [%s]\n",
		flashset_nic_restore("eth0") ? "fail" : " ok ",
		flashset_led_restore()       ? "fail" : " ok ");
	}

	return 0;
}
