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

#include <drivers/gpio/k210.h>
#include <drivers/gpio/gpio_driver.h>

EMBOX_UNIT_INIT(k210_gpio_init);

static int k210_gpio_init(void){
	return 0;
}
