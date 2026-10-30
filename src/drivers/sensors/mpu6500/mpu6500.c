/**
 * @file
 * @brief Identify MPU6500 by reading WHO_AM_I.
 * @author Dmitrii Piliuk
 * @version
 * @date 28.10.2026
 */

#include <errno.h>
#include <stdint.h>

#include <util/log.h>

#include "mpu6500.h"
#include "mpu6500_transfer.h"

#define MPU6500_WHO_AM_I       0x75
#define MPU6500_WHO_AM_I_VALUE 0x70

static void mpu6500_identification_diagnostics(void) {
	static const uint8_t regs[] = {0x75, 0x75, 0x75, 0x6b, 0x6c};
	uint8_t value;
	unsigned int i;
	int ret;

	/* Read-only diagnostics: repeated ID and power-management registers. */
	for (i = 0; i < sizeof(regs) / sizeof(regs[0]); i++) {
		ret = mpu6500_readb(regs[i], &value);
		if (ret) {
			log_error("MPU6500 register 0x%02x read failed: %d", regs[i], ret);
			return;
		}
		log_info("MPU6500 diagnostic register 0x%02x = 0x%02x",
		    regs[i], value);
	}
}

int mpu6500_init(void) {
	uint8_t value;
	int ret;

	ret = mpu6500_hw_init();
	if (ret) {
		log_error("MPU6500 SPI initialization failed: %d", ret);
		return ret;
	}

	ret = mpu6500_readb(MPU6500_WHO_AM_I, &value);
	if (ret) {
		log_error("MPU6500 WHO_AM_I read failed: %d", ret);
		return ret;
	}

	log_info("MPU6500 WHO_AM_I = 0x%02x", value);
	if (value != MPU6500_WHO_AM_I_VALUE) {
		log_error("MPU6500 device ID mismatch: expected 0x%02x, got 0x%02x",
		    MPU6500_WHO_AM_I_VALUE, value);
		mpu6500_identification_diagnostics();
		return -ENODEV;
	}

	return 0;
}
