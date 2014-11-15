/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.10.2014
 */

#include <errno.h>
#include <string.h>
#include <ifaddrs.h>
#include <net/inetdevice.h>

#include <drivers/flash/flash_dev.h>

extern const struct flash_dev stm32_flash;

#define FLASHSET_MAGIC 0x5500AAAA
struct flashset_nic_settings {
	uint32_t fsn_magic;
	struct sockaddr fsn_addr;
	struct sockaddr fsn_mask;
	unsigned char fsn_mac[MAX_ADDR_LEN];
};

static int flashset_nic_store(const char *nic_name, struct flash_dev *flashdev, unsigned long offset, unsigned int block) {
	struct flashset_nic_settings fl_nic_settings;
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

	fl_nic_settings.fsn_magic = FLASHSET_MAGIC;
	memcpy(&fl_nic_settings.fsn_addr, nic_ifa->ifa_addr, sizeof(fl_nic_settings.fsn_addr));
	memcpy(&fl_nic_settings.fsn_mask, nic_ifa->ifa_netmask, sizeof(fl_nic_settings.fsn_mask));
	memcpy(&fl_nic_settings.fsn_mac, iface_dev->dev->dev_addr, sizeof(fl_nic_settings.fsn_mac));

	if ((errcode = flash_erase(flashdev, 0))) {
		goto outerr;
	}

	errcode = flash_write(flashdev, 0, &fl_nic_settings, sizeof(fl_nic_settings));
outerr:
	return errcode;
}

static int flashset_nic_restore(const char *nic_name, struct flash_dev *flashdev, unsigned long offset, unsigned int block) {
	struct flashset_nic_settings fl_nic_settings;
	struct in_device *iface_dev;
	int errcode;

	if (0 > (errcode = flash_read(flashdev, offset, &fl_nic_settings, sizeof(fl_nic_settings)))) {
		goto outerr;
	}

	if (fl_nic_settings.fsn_magic != FLASHSET_MAGIC) {
		errcode = -ENOENT;
		goto outerr;
	}

	iface_dev = inetdev_get_by_name(nic_name);
	if (!iface_dev) {
		errcode = -ESRCH;
		goto outerr;
	}

	if ((errcode = inetdev_set_addr(iface_dev, ((struct sockaddr_in *) &fl_nic_settings.fsn_addr)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = inetdev_set_mask(iface_dev, ((struct sockaddr_in *) &fl_nic_settings.fsn_mask)->sin_addr.s_addr))) {
		goto outerr;
	}

	if ((errcode = netdev_set_macaddr(iface_dev->dev, &fl_nic_settings.fsn_mac))) {
		goto outerr;
	}

	errcode = 0;
outerr:
	return errcode;
}

int main(int argc, char *argv[]) {
	struct flash_dev *flashdev = (struct flash_dev *) &stm32_flash;
	int errcode;

	if (0 == strcmp(argv[1], "store")) {
		errcode = flashset_nic_store("eth0", flashdev, 0, 0);
	} else if (0 == strcmp(argv[1], "restore")) {
		errcode = flashset_nic_restore("eth0", flashdev, 0, 0);
	}

	return errcode;
}

