/**
 * @file
 *
 * @date 15.05.2016
 * @author Anton Bondarev
 */
#include <stdint.h>
#include <errno.h>

#include <util/log.h>

#include <asm/io.h>

#include "es1370.h"

#define RESET_AND_POWER_DOWN     0x16
#define PD                              0x02
#define RST                             0x01

#define AD_INPUT_SELECT          0x18
#define MIC_AMP_GAIN             0x19


static uint32_t ak4531_base_addr;
static uint32_t ak4531_status_reg;
static uint32_t ak4531_poll_reg;


static uint8_t mixer_values[0x20] = {
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, /* 0x00 - 0x07 */
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, /* 0x08 - 0x0f */
	0x7e, 0x3d, 0x01, 0x01, 0x00, 0x00, 0x03, 0x00, /* 0x10 - 0x17 */
	0x00, 0x01										/* 0x18 - 0x19 */
};

static int ak4531_finished(void) {
	int i;
	uint32_t cstat;
	for (i = 0; i < 0x40000; i++) {
		cstat = in32(ak4531_status_reg);
		if (!(cstat & STAT_CSTAT)) {
			return 1;
		}
	}
	return 0;
}
static int ak4531_write(uint8_t idx, uint8_t data) {

	if (!ak4531_finished()) {
		return -EBUSY;
	}

	out16((uint16_t)idx << 8 | data, ak4531_base_addr);

	return 0;
}

int ak4531_init(uint32_t base_addr, uint32_t status_addr, uint32_t poll_addr) {
	int i;

	ak4531_base_addr = base_addr;
	ak4531_poll_reg = poll_addr;
	ak4531_status_reg = status_addr;

	for (i=0; i<100; i++) {
		in8(poll_addr);
	}

	ak4531_write(RESET_AND_POWER_DOWN, PD | RST); /* no RST, PD */

	for (i=0; i<100; i++) {
		in8(poll_addr);
	}
#if 0
	/* CODEC ADC and CODEC DAC use {LR,B}CLK2 and run off the LRCLK2 PLL; program DAC_SYNC=0!! */
	ak4531_write( 0x17, 0);
#endif
	ak4531_write(AD_INPUT_SELECT, 0x00); /* recording source is mixer */

	for (i = 0 ; i < sizeof(mixer_values); i++) {
		if (ak4531_write(i, mixer_values[i]) < 0) {
			return -1;
		}
	}

	return 0;
}
