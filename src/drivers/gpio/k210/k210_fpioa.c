/**
 * @file k210_gpio.c
 * @brief
 * @author sksat <sksat@sksat.net>
 * @version 0.1
 * @date 2020-08-15
 */

#include <assert.h>
#include <util/log.h>

#include <embox/unit.h>
#include <util/bit.h>
#include <util/array.h>

#include <drivers/gpio/k210/fpioa.h>

EMBOX_UNIT_INIT(k210_fpioa_init);

// TODO: add sysctl driver

// central clock enable(sysctl: 0x28)
volatile sysctl_clock_enable_central* const clk_en_cent = (volatile sysctl_clock_enable_central*) SYSCTL_BASE_ADDR + 0x28;
volatile sysctl_clock_enable_peripheral* const clk_en_peri = (volatile sysctl_clock_enable_peripheral*) SYSCTL_BASE_ADDR + 0x2c;

static int k210_fpioa_init(void){

	// enable bus clock
	clk_en_cent->apb0 = 1;

	// enable device clock
	clk_en_peri ->fpioa = 1;

	return 0;
}
