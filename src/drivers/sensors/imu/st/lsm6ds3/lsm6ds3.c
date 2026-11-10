/**
 * @file
 * @brief
 *
 * @date 06.11.2026
 * @author Anton Bondarev
 */
#include <unistd.h>
#include <util/log.h>

#include "lsm6ds3.h"
#include "lsm6ds3_transfer.h"

#define LSM6DS3_WHO_AM_I     0xf


#define LSM6DS3_WHO_AM_I_VALUE 0x69

extern struct lsm6ds3_dev lsm6ds3_dev0;

int lsm6ds3_init(void) {
	uint8_t tmp;
	struct lsm6ds3_dev *dev = &lsm6ds3_dev0;

	if (lsm6ds3_hw_init(dev) < 0) {
		log_error("lsm6ds3 hw init failed!");
		return -1;
	}

	if (lsm6ds3_readb(dev, LSM6DS3_WHO_AM_I, &tmp) < 0) {
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != LSM6DS3_WHO_AM_I_VALUE) {
		log_error("LSM6DS3 Device ID mismatch! 0x%2x", tmp);
		return -1;
	}

	return 0;
}
