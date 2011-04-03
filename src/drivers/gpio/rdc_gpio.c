/**
 * @file
 * @brief RDC GPIO driver
 *
 * @date 31.03.11
 * @author Nikolay Korotkiy
 */
#include <asm/io.h>
#include <linux/init.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(gpio_init);

#define RDC_CONTROL  0x80003848
#define RDC_DATA     0x8000384c

#define GPIO_RTCRD    (1 << 16) /* Red LED */
#define GPIO_RTCAS    (1 << 15) /* Reset button */

static unsigned long g_last_value = 0xffffffff;

static inline void set_control(unsigned long mask) {
	unsigned long tmp;
	/* Select control register */
	out32(RDC_CONTROL, 0xcf8);
	tmp = inl(0xcfc);
	/* raise to set GPIO function */
	tmp |= mask;
	out32(tmp, 0xcfc);
}

static inline void set_data(unsigned long mask) {
	out32(RDC_DATA, 0xcf8);
	g_last_value |= mask;
	out32(g_last_value, 0xcfc);
}

static inline void clear_data(unsigned long mask) {
	out32(RDC_DATA, 0xcf8);
	g_last_value &= ~mask;
	out32(g_last_value, 0xcfc);
}

int gpio_get_value(unsigned long mask) {
	unsigned long tmp;
	out32(RDC_DATA, 0xcf8);
	tmp = in32(0xcfc);
	return (tmp & mask) ? 1 : 0;
}

void gpio_input(unsigned long mask) {
	/* raise logic level to turn off pull-down */
	set_data(mask);
	/* select as GPIO function */
	set_control(mask);
}

void gpio_output(unsigned long mask, int value) {
	if (value) {
		set_data(mask);
	} else {
		clear_data(mask);
	}
	set_control(mask);
}

void gpio_set_value(unsigned long mask, int value) {
	if (value) {
		set_data(mask);
	} else {
		clear_data(mask);
	}
}

static int __init gpio_init(void) {
	/* Example: blink led
	set_control(GPIO_RTCRD);
	out32(RDC_DATA, 0xcf8);
	int state = 1;
	while(1) {
		if (state) {
			out32(0, 0xcfc);
			state = 0;
		} else {
			out32(GPIO_RTCRD, 0xcfc);
			state = 1;
		}
	}*/
	return 0;
}

