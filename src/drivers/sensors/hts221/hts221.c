/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#include <unistd.h>
#include <util/log.h>

#include "hts221.h"
#include "hts221_transfer.h"

#define HTS221_WHO_AM_I		0xf
#define HTS221_AV_CONF_REG	0x10
#define HTS221_CTRL_REG(i)	(0x20 + i - 1)
#define HTS221_STATUS_REG	0x27
#define HTS221_HUMIDITY_OUT_L	0x28
#define HTS221_HUMIDITY_OUT_H	0x29
#define HTS221_TEMP_OUT_L	0x2A
#define HTS221_TEMP_OUT_H	0x2B
#define HTS221_H0_RH_X2		0x30
#define HTS221_H1_RH_X2		0x31
#define HTS221_T0_DEGC_X8	0x32
#define HTS221_T1_DEGC_X8	0x33
#define HTS221_T0_1_DEGC_X8_MSB	0x35
#define HTS221_H0_OUT_L		0x36
#define HTS221_H0_OUT_H		0x37
#define HTS221_H1_OUT_L		0x3a
#define HTS221_H1_OUT_H		0x3b
#define HTS221_T0_OUT_L		0x3c
#define HTS221_T0_OUT_H		0x3d
#define HTS221_T1_OUT_L		0x3e
#define HTS221_T1_OUT_H		0x3f


#define HTS221_WHO_AM_I_VALUE 0xbc

extern struct hts221_dev hts221_dev0;

static int16_t H0_rH_x2, dH_rH_x2, H0, dH, T0_degC_x8, dT_degC_x8, T0, dT;

int16_t hts221_get_humidity(void) {
	uint8_t l, h;
	int32_t H;
	struct hts221_dev *dev = &hts221_dev0;
	hts221_readb(dev, HTS221_HUMIDITY_OUT_L, &l);
	hts221_readb(dev, HTS221_HUMIDITY_OUT_H, &h);

	H = l | (int16_t) (h << 8);
	return ((H - H0) * dH_rH_x2 + (int32_t)dH * H0_rH_x2 + dH) / (2 * dH);
}

int16_t hts221_get_temp_x10(void) {
	uint8_t l, h;
	int32_t T;
	struct hts221_dev *dev = &hts221_dev0;
	hts221_readb(dev, HTS221_TEMP_OUT_L, &l);
	hts221_readb(dev, HTS221_TEMP_OUT_H, &h);

	T = l | (int16_t) (h << 8);
	return (((T - T0) * dT_degC_x8 + (int32_t)dT * T0_degC_x8) * 5 + 2 * dT) / (4 * dT);
}

int hts221_init(void) {
	int i;
	uint8_t tmp;
	struct hts221_dev *dev = &hts221_dev0;

	if (hts221_hw_init(dev) < 0) {
		log_error("HTS221 hw init failed!");
		return -1;
	}

	if (hts221_readb(dev, HTS221_WHO_AM_I, &tmp) < 0) {
		log_error("HTS221 readb failed!");
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != HTS221_WHO_AM_I_VALUE) {
		log_error("HTS221 Device ID mismatch! %2x", tmp);
		return -1;
	}

	tmp = 0x3;		/* Humidity average number is 32 */
	tmp |= (0x3 << 3);	/* Temperature average number is 16 */
	hts221_writeb(dev, HTS221_AV_CONF_REG, tmp);

	tmp = 0x1;		/* Output data rate is 1 Hz */
	tmp |= (1 << 2);	/* Block data update until MSB reading after LSB */
	tmp |= (1 << 7);	/* Active mode */
	hts221_writeb(dev, HTS221_CTRL_REG(1), tmp);

	for (i = 1; i <= 3; i++) {
		hts221_readb(dev, HTS221_CTRL_REG(i), &tmp);
		log_info("CTRL_REG%d = 0x%02x", i, tmp);
	}
	hts221_readb(dev, HTS221_STATUS_REG, &tmp);
	log_info("STATUS_REG = 0x%02x", tmp);

	hts221_readb(dev, HTS221_H0_RH_X2, &tmp);
	H0_rH_x2 = tmp;
	hts221_readb(dev, HTS221_H1_RH_X2, &tmp);
	dH_rH_x2 = tmp-H0_rH_x2;

	hts221_readb(dev, HTS221_H0_OUT_L, &tmp);
	H0 = tmp;
	hts221_readb(dev, HTS221_H0_OUT_H, &tmp);
	H0 |= (int16_t)tmp<<8;

	hts221_readb(dev, HTS221_H1_OUT_L, &tmp);
	dH = tmp;
	hts221_readb(dev, HTS221_H1_OUT_H, &tmp);
	dH |= (int16_t)tmp<<8;
	dH -= H0;

	hts221_readb(dev, HTS221_T0_DEGC_X8, &tmp);
	T0_degC_x8 = tmp;
	hts221_readb(dev, HTS221_T1_DEGC_X8, &tmp);
	dT_degC_x8 = tmp;
	hts221_readb(dev, HTS221_T0_1_DEGC_X8_MSB, &tmp);
	T0_degC_x8 |= (((uint16_t)tmp&0x3)<<8);
	dT_degC_x8 |= (((uint16_t)tmp&0xc)<<6);
	dT_degC_x8 -= T0_degC_x8;

	hts221_readb(dev, HTS221_T0_OUT_L, &tmp);
	T0 = tmp;
	hts221_readb(dev, HTS221_T0_OUT_H, &tmp);
	T0 |= (int16_t)tmp<<8;

	hts221_readb(dev, HTS221_T1_OUT_L, &tmp);
	dT = tmp;
	hts221_readb(dev, HTS221_T1_OUT_H, &tmp);
	dT |= (int16_t)tmp<<8;
	dT -= T0;

	return 0;
}
