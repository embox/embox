/**
 * @file
 * @brief
 *
 * @date 06.02.2020
 * @author Dmitry Pilyuk
 */
#include <unistd.h>

#include <util/log.h>

#include "mpu6000.h"
#include "mpu6000_transfer.h"

#define MPU6000_WHO_AM_I 0x75

#define MPU6000_WHO_AM_I_VALUE 0x68

extern struct mpu6000_dev mpu6000_dev0;

int mpu6000_init(void) {
	uint8_t tmp;
	struct mpu6000_dev *dev = &mpu6000_dev0;

	if (mpu6000_hw_init(dev) < 0) {
		log_error("MPU6000 hw init failed!");
		return -1;
	}

	if (mpu6000_readb(dev, MPU6000_WHO_AM_I, &tmp) < 0) {
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != MPU6000_WHO_AM_I_VALUE) {
		log_error("MPU6000 Device ID mismatch! %2x", tmp);
		return -1;
	}
	return 0;
}
