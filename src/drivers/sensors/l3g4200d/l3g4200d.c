/**
 * @file
 * @brief
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */
#include <unistd.h>
#include <util/log.h>

#include "l3g4200d.h"
#include "l3g4200d_transfer.h"

#define L3G4200D_WHO_AM_I     0xf
#define L3G4200D_CTRL_REG(i)  (0x20 + i)
#define L3G4200D_STATUS_REG   0x27
#define L3G4200D_OUT_X_L      0x28
#define L3G4200D_OUT_X_H      0x29
#define L3G4200D_OUT_Y_L      0x2A
#define L3G4200D_OUT_Y_H      0x2B
#define L3G4200D_OUT_Z_L      0x2C
#define L3G4200D_OUT_Z_H      0x2D

#define L3G4200D_WHO_AM_I_VALUE 0xd3

extern struct l3g4200d_dev l3g4200d_dev0;

int16_t l3g4200d_get_angular_rate_x(void) {
	uint8_t l, h;
	struct l3g4200d_dev *dev = &l3g4200d_dev0;
	l3g4200d_readb(dev, L3G4200D_OUT_X_L, &l);
	l3g4200d_readb(dev, L3G4200D_OUT_X_H, &h);
	return l | ((int16_t) h << 8);
}

int16_t l3g4200d_get_angular_rate_y(void) {
	uint8_t l, h;
	struct l3g4200d_dev *dev = &l3g4200d_dev0;
	l3g4200d_readb(dev, L3G4200D_OUT_Y_L, &l);
	l3g4200d_readb(dev, L3G4200D_OUT_Y_H, &h);
	return l | ((int16_t) h << 8);
}

int16_t l3g4200d_get_angular_rate_z(void) {
	uint8_t l, h;
	struct l3g4200d_dev *dev = &l3g4200d_dev0;
	l3g4200d_readb(dev, L3G4200D_OUT_Z_L, &l);
	l3g4200d_readb(dev, L3G4200D_OUT_Z_H, &h);
	return l | ((int16_t) h << 8);
}

int l3g4200d_init(void) {
	int i;
	uint8_t tmp;
	struct l3g4200d_dev *dev = &l3g4200d_dev0;

	if (l3g4200d_hw_init(dev) < 0) {
		log_error("L3G4200D hw init failed!");
		return -1;
	}

	if (l3g4200d_readb(dev, L3G4200D_WHO_AM_I, &tmp) < 0) {
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != L3G4200D_WHO_AM_I_VALUE) {
		log_error("L3G4200D Device ID mismatch! %2x", tmp);
		return -1;
	}

	tmp = 0x7; /* Enable X, Y, Z */
	tmp |= (1 << 3); /* Enable power */
	l3g4200d_writeb(dev, L3G4200D_CTRL_REG(0), tmp);

	for (i = 0; i < 5; i++) {
		l3g4200d_readb(dev, L3G4200D_CTRL_REG(i), &tmp);
		log_info("CTRL_REG%d = 0x%02x", i + 1, tmp);
	}
	l3g4200d_readb(dev, L3G4200D_STATUS_REG, &tmp);
	log_info("STATUS_REG = 0x%02x", tmp);

	return 0;
}
