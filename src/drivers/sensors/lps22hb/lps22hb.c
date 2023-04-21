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

#include "lps22hb.h"
#include "lps22hb_transfer.h"

#define LPS22HB_INTERRUPT_CFG	0x0b
#define LPS22HB_THS_P_L		0x0c
#define LPS22HB_THS_P_H		0x0d
#define LPS22HB_WHO_AM_I	0xf
#define LPS22HB_CTRL_REG(i)	(0x10 + i - 1)
#define LPS22HB_FIFO_CTRL	0x14
#define LPS22HB_REF_P_XL	0x15
#define LPS22HB_REF_P_L		0x16
#define LPS22HB_REF_P_H		0x17
#define LPS22HB_RPDS_L		0x18
#define LPS22HB_RPDS_H		0x19
#define LPS22HB_RES_CONF	0x1a
#define LPS22HB_INT_SOURCE	0x25
#define LPS22HB_FIFO_STATUS	0x26
#define LPS22HB_STATUS_REG	0x27
#define LPS22HB_PRESS_OUT_XL	0x28
#define LPS22HB_PRESS_OUT_L	0x29
#define LPS22HB_PRESS_OUT_H	0x2a
#define LPS22HB_TEMP_OUT_L	0x2b
#define LPS22HB_TEMP_OUT_H	0x2c
#define LPS22HB_LPFP_RES	0x33


#define LPS22HB_WHO_AM_I_VALUE 0xb1

extern struct lps22hb_dev lps22hb_dev0;

int32_t lps22hb_get_pressure_4096(void) {
	struct lps22hb_dev *dev = &lps22hb_dev0;
	union {int32_t val; uint8_t b[4];} p;

	lps22hb_readb(dev, LPS22HB_PRESS_OUT_XL,&(p.b[0]));
	lps22hb_readb(dev, LPS22HB_PRESS_OUT_L, &(p.b[1]));
	lps22hb_readb(dev, LPS22HB_PRESS_OUT_H, &(p.b[2]));
	p.b[3] = (p.b[2] & 0x80)? 0xff: 0; // sign spread

	return le32toh(p.val);
}

int16_t lps22hb_get_temp_x100(void) {
	struct lps22hb_dev *dev = &lps22hb_dev0;
	union {int16_t val; uint8_t b[2];}t;
	lps22hb_readb(dev, LPS22HB_TEMP_OUT_L, &(t.b[0]));
	lps22hb_readb(dev, LPS22HB_TEMP_OUT_H, &(t.b[1]));

	return le16toh(t.val);
}

int lps22hb_init(void) {
	int i;
	uint8_t tmp;
	struct lps22hb_dev *dev = &lps22hb_dev0;

	if (lps22hb_hw_init(dev) < 0) {
		log_error("LPS22HB hw init failed!");
		return -1;
	}

	if (lps22hb_readb(dev, LPS22HB_WHO_AM_I, &tmp) < 0) {
		log_error("LPS22HB readb failed!");
		return -1;
	}
	log_info("WHO_AM_I = 0x%02x", tmp);
	if (tmp != LPS22HB_WHO_AM_I_VALUE) {
		log_error("LPS22HB Device ID mismatch! %2x", tmp);
		return -1;
	}

	tmp = 0x10;		/* Output data rate is 1 Hz */
	lps22hb_writeb(dev, LPS22HB_CTRL_REG(1), tmp);

	for (i = 1; i <= 3; i++) {
		lps22hb_readb(dev, LPS22HB_CTRL_REG(i), &tmp);
		log_info("CTRL_REG%d = 0x%02x", i, tmp);
	}
	lps22hb_readb(dev, LPS22HB_STATUS_REG, &tmp);
	log_info("STATUS_REG = 0x%02x", tmp);
	return 0;
}
