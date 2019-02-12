/**
 * @file
 *
 * @date Apr 2, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_VIDEO_LYNXFB_DDK750_POWER_H_
#define SRC_DRIVERS_VIDEO_LYNXFB_DDK750_POWER_H_

#include "ddk750_reg.h"

typedef enum _DPMS_t {
	crtDPMS_ON = 0x0,
	crtDPMS_STANDBY = 0x1,
	crtDPMS_SUSPEND = 0x2,
	crtDPMS_OFF = 0x3,
}
DPMS_t;

#define setDAC(off) {   \
	poke32(MISC_CTRL, \
			(peek32(MISC_CTRL) & ~MISC_CTRL_DAC_POWER_OFF) | (off)); \
}

extern void ddk750_set_dpms(DPMS_t state);
extern void sm750_set_power_mode(unsigned int powerMode);
extern void sm750_set_current_gate(unsigned int gate);

/*
 * This function enable/disable the 2D engine.
 */
extern void sm750_enable_2d_engine(unsigned int enable);

/*
 * This function enable/disable the DMA Engine
 */
extern void sm750_enable_dma(unsigned int enable);

/*
 * This function enable/disable the GPIO Engine
 */
extern void sm750_enable_gpio(unsigned int enable);

/*
 * This function enable/disable the I2C Engine
 */
extern void sm750_enable_i2c(unsigned int enable);

#endif /* SRC_DRIVERS_VIDEO_LYNXFB_DDK750_POWER_H_ */
