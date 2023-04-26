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

#include "lis3mdl.h"
#include "lis3mdl_transfer.h"

#define LIS3MDL_WHO_AM_I	0xf
#define LIS3MDL_CTRL_REG(i)	(0x20 + i - 1)
#define LIS3MDL_STATUS_REG	0x27
#define LIS3MDL_OUT_X_L		0x28
#define LIS3MDL_OUT_X_H		0x29
#define LIS3MDL_OUT_Y_L		0x2a
#define LIS3MDL_OUT_Y_H		0x2b
#define LIS3MDL_OUT_Z_L		0x2c
#define LIS3MDL_OUT_Z_H		0x2d
#define LIS3MDL_TEMP_OUT_L	0x2e
#define LIS3MDL_TEMP_OUT_H	0x2f
#define LIS3MDL_INT_CFG		0x30
#define LIS3MDL_INT_SRC		0x31
#define LIS3MDL_INT_THS_L	0x32
#define LIS3MDL_INT_THS_H	0x33


#define LIS3MDL_WHO_AM_I_VALUE 0x3d

#define LIS3MDL_FS4_SENSITIVITY		6842
#define LIS3MDL_FS8_SENSITIVITY		3421
#define LIS3MDL_FS12_SENSITIVITY	2281
#define LIS3MDL_FS16_SENSITIVITY	1711

#define LIS3MDL_T_OFFSET		(25*8)

extern struct lis3mdl_dev lis3mdl_dev0;

static int sens;

void lis3mdl_get_data(struct lis3mdl_s *data) {
	struct lis3mdl_dev *dev = &lis3mdl_dev0;
	union {int16_t val; uint8_t b[2];}m;
	lis3mdl_readb(dev, LIS3MDL_OUT_X_L, &(m.b[0]));
	lis3mdl_readb(dev, LIS3MDL_OUT_X_H, &(m.b[1]));
	data->x = le16toh(m.val);
	lis3mdl_readb(dev, LIS3MDL_OUT_Y_L, &(m.b[0]));
	lis3mdl_readb(dev, LIS3MDL_OUT_Y_H, &(m.b[1]));
	data->y = le16toh(m.val);
	lis3mdl_readb(dev, LIS3MDL_OUT_Z_L, &(m.b[0]));
	lis3mdl_readb(dev, LIS3MDL_OUT_Z_H, &(m.b[1]));
	data->z = le16toh(m.val);
	data->sens = sens;
}

int16_t lis3mdl_get_temp_x8(void) {
	struct lis3mdl_dev *dev = &lis3mdl_dev0;
	union {int16_t val; uint8_t b[2];}t;
	lis3mdl_readb(dev, LIS3MDL_TEMP_OUT_L, &(t.b[0]));
	lis3mdl_readb(dev, LIS3MDL_TEMP_OUT_H, &(t.b[1]));

	return le16toh(t.val) + LIS3MDL_T_OFFSET;
}

int lis3mdl_init(void) {
	int i;
	uint8_t tmp;
	struct lis3mdl_dev *dev = &lis3mdl_dev0;

	if (lis3mdl_hw_init(dev) < 0) {
		log_error("LIS3MDL hw init failed!");
		return -1;
	}

	if (lis3mdl_readb(dev, LIS3MDL_WHO_AM_I, &tmp) < 0) {
		log_error("LIS3MDL readb failed!");
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != LIS3MDL_WHO_AM_I_VALUE) {
		log_error("LIS3MDL Device ID mismatch! %2x", tmp);
		return -1;
	}

	tmp = 0x80;	/* Temperature sensor enable */
	tmp |= 1<<2;	/* Output data rate 1.25 Hz */
	lis3mdl_writeb(dev, LIS3MDL_CTRL_REG(1), tmp);

	tmp = 0 << 5;	/* Full-scale +-4 gauss */
	sens = LIS3MDL_FS4_SENSITIVITY;
	lis3mdl_writeb(dev, LIS3MDL_CTRL_REG(2), tmp);

	tmp = 0;	/* Continuous-conversion mode */
	lis3mdl_writeb(dev, LIS3MDL_CTRL_REG(3), tmp);

	tmp = 0x40;	/* Block data update for magnetic data */
	lis3mdl_writeb(dev, LIS3MDL_CTRL_REG(5), tmp);

	for (i = 1; i <= 3; i++) {
		lis3mdl_readb(dev, LIS3MDL_CTRL_REG(i), &tmp);
		log_info("CTRL_REG%d = 0x%02x", i, tmp);
	}
	lis3mdl_readb(dev, LIS3MDL_STATUS_REG, &tmp);
	log_info("STATUS_REG = 0x%02x", tmp);
	return 0;
}
