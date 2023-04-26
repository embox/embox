/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#include <unistd.h>
#include <util/log.h>
#include <endian.h>

#include "lsm6dsl.h"
#include "lsm6dsl_transfer.h"

#define LSM6DSL_WHO_AM_I	0xf
#define LSM6DSL_CTRL_REG(i)	(0x10 + i - 1)
#define LSM6DSL_STATUS_REG	0x1e
#define LSM6DSL_OUT_TEMP_L	0x20
#define LSM6DSL_OUT_TEMP_H	0x21
#define LSM6DSL_OUT_X_L_G	0x22
#define LSM6DSL_OUT_X_H_G	0x23
#define LSM6DSL_OUT_Y_L_G	0x24
#define LSM6DSL_OUT_Y_H_G	0x25
#define LSM6DSL_OUT_Z_L_G	0x26
#define LSM6DSL_OUT_Z_H_G	0x27
#define LSM6DSL_OUT_X_L_XL	0x28
#define LSM6DSL_OUT_X_H_XL	0x29
#define LSM6DSL_OUT_Y_L_XL	0x2a
#define LSM6DSL_OUT_Y_H_XL	0x2b
#define LSM6DSL_OUT_Z_L_XL	0x2c
#define LSM6DSL_OUT_Z_H_XL	0x2d
#define LSM6DSL_TIMESTAMP0_REG	0x40
#define LSM6DSL_TIMESTAMP1_REG	0x41
#define LSM6DSL_TIMESTAMP2_REG	0x42
#define LSM6DSL_WAKE_UP_DUR	0x5c

#define LSM6DSL_WHO_AM_I_VALUE 0x6A

#define LSM6DSL_TEMP_OFFSET		(25*256)
#define LSM6DSL_TIMEST_STEP		25

extern struct lsm6dsl_dev lsm6dsl_dev0;

void lsm6dsl_get_data(struct lsm6dsl_s *data) {
	struct lsm6dsl_dev *dev = &lsm6dsl_dev0;
	union {int16_t val; uint8_t b[2];}m;
	lsm6dsl_readb(dev, LSM6DSL_OUT_X_L_G, &(m.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_X_H_G, &(m.b[1]));
	data->g_x = le16toh(m.val);
	lsm6dsl_readb(dev, LSM6DSL_OUT_Y_L_G, &(m.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_Y_H_G, &(m.b[1]));
	data->g_y = le16toh(m.val);
	lsm6dsl_readb(dev, LSM6DSL_OUT_Z_L_G, &(m.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_Z_H_G, &(m.b[1]));
	data->g_z = le16toh(m.val);
	lsm6dsl_readb(dev, LSM6DSL_OUT_X_L_XL, &(m.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_X_H_XL, &(m.b[1]));
	data->xl_x = le16toh(m.val);
	lsm6dsl_readb(dev, LSM6DSL_OUT_Y_L_XL, &(m.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_Y_H_XL, &(m.b[1]));
	data->xl_y = le16toh(m.val);
	lsm6dsl_readb(dev, LSM6DSL_OUT_Z_L_XL, &(m.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_Z_H_XL, &(m.b[1]));
	data->xl_z = le16toh(m.val);
}

int16_t lsm6dsl_get_temp_x256(void) {
	struct lsm6dsl_dev *dev = &lsm6dsl_dev0;
	union {int16_t val; uint8_t b[2];}t;
	lsm6dsl_readb(dev, LSM6DSL_OUT_TEMP_L, &(t.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_OUT_TEMP_H, &(t.b[1]));

	return le16toh(t.val) + LSM6DSL_TEMP_OFFSET;
}

int32_t lsm6dsl_get_timestamp_us(void) {
	struct lsm6dsl_dev *dev = &lsm6dsl_dev0;
	union {int32_t val; uint8_t b[4];}t;
	lsm6dsl_readb(dev, LSM6DSL_TIMESTAMP0_REG, &(t.b[0]));
	lsm6dsl_readb(dev, LSM6DSL_TIMESTAMP1_REG, &(t.b[1]));
	lsm6dsl_readb(dev, LSM6DSL_TIMESTAMP2_REG, &(t.b[2]));
	t.b[3] = 0; // TO DO: overflow catch!

	return le32toh(t.val)*LSM6DSL_TIMEST_STEP;
}

int lsm6dsl_init(void) {
	int i;
	uint8_t tmp;
	struct lsm6dsl_dev *dev = &lsm6dsl_dev0;

	if (lsm6dsl_hw_init(dev) < 0) {
		log_error("LSM6DSL hw init failed!");
		return -1;
	}

	if (lsm6dsl_readb(dev, LSM6DSL_WHO_AM_I, &tmp) < 0) {
		log_error("LSM6DSL readb failed!");
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != LSM6DSL_WHO_AM_I_VALUE) {
		log_error("LSM6DSL Device ID mismatch! %2x", tmp);
		return -1;
	}

	tmp = 0x20;	/* Accelerometer data rate 26 Hz (low power) */
	tmp |= 0<<2;	/* Accelerometer full-scale ±2 g */
	lsm6dsl_writeb(dev, LSM6DSL_CTRL_REG(1), tmp);

	tmp = 0x20;	/* Gyroscope data rate 26 Hz (low power) */
	tmp |= 0<<2;	/* Gyroscope full-scale 250 dps */
	lsm6dsl_writeb(dev, LSM6DSL_CTRL_REG(2), tmp);

	tmp = 0x40;	/* Block data update */
	tmp |= 0x4;	/* Register address autoincrement */
	lsm6dsl_writeb(dev, LSM6DSL_CTRL_REG(3), tmp);

//	tmp = 0x80;	/* DEN value stored in LSB of X-axis */
//	tmp = 0x40;	/* DEN value stored in LSB of Y-axis */
//	tmp = 0x20;	/* DEN value stored in LSB of Z-axis */
//	tmp |= 0x10;	/* DEN pin info stamped in the accelerometer axis selected by bits [7:5] */
//	lsm6dsl_writeb(dev, LSM6DSL_CTRL_REG(9), tmp);

	tmp = 0x10;	/* Reset timestamp counter */
	lsm6dsl_writeb(dev, LSM6DSL_WAKE_UP_DUR, tmp);
	tmp = 0xaa;	/* Reset timestamp counter */
	lsm6dsl_writeb(dev, LSM6DSL_TIMESTAMP2_REG, tmp);
	tmp = 0x20;	/* Enable timestamp counter */
	lsm6dsl_writeb(dev, LSM6DSL_CTRL_REG(10), tmp);

	for (i = 1; i <= 10; i++) {
		lsm6dsl_readb(dev, LSM6DSL_CTRL_REG(i), &tmp);
		log_info("CTRL_REG%d = 0x%02x", i, tmp);
	}
	lsm6dsl_readb(dev, LSM6DSL_STATUS_REG, &tmp);
	log_info("STATUS_REG = 0x%02x", tmp);
	return 0;
}
