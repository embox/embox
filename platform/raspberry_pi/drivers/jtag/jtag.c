/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */

#include <embox/unit.h>
#include <kernel/printk.h>
#include <framework/mod/options.h>

#include <drivers/gpio/gpio.h>
#include <drivers/gpio/bcm283x/bcm283x_gpio.h>

#define CONFIG_DEFAULT  OPTION_GET(NUMBER,gpio_jtag_config)

#define OPTIONS 	3

int gpio_jtag(uint8_t config) {
    uint32_t pins_GFAlt5[OPTIONS] = {
		( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 6 ) | ( 1 << 12 ) | ( 1 << 13 )
	,	0x00
	,	( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 6 ) | ( 1 << 12 ) | ( 1 << 13 )
	};
	uint32_t pins_GFAlt4[OPTIONS] = {
		( 1 << 22 )
	,	( 1 << 22 ) | ( 1 << 23) | ( 1 << 24 ) | ( 1 << 25 ) | ( 1 << 26 ) | ( 1 << 27 )
	,	0x00
	};

	if(config < OPTIONS) {
		if(pins_GFAlt5[config]) {
			gpio_setup_mode(GPIO_PORT_A, pins_GFAlt5[config], GPIO_MODE_ALT_SET(GFAlt5) );
		}
		if(pins_GFAlt4[config]) {
			gpio_setup_mode(GPIO_PORT_A, pins_GFAlt4[config], GPIO_MODE_ALT_SET(GFAlt4) );
		}
		return 0;
	} else {
		return -1;
	}
}

static int gpio_jtag_init(void) {
    int ret;

	if(CONFIG_DEFAULT >= 0) {		
		ret = gpio_jtag(CONFIG_DEFAULT);
    	printk(" Opt %d ",CONFIG_DEFAULT);
	}
	
	return ret;
}

EMBOX_UNIT_INIT(gpio_jtag_init);
