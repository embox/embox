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

volatile k210_gpio_t* const gpio = (volatile k210_gpio_t*) GPIO_BASE_ADDR;

extern volatile sysctl_clock_enable_central* const clk_en_cent;
extern volatile sysctl_clock_enable_peripheral* const clk_en_peri;

typedef enum {
	INPUT,
	INPUT_PULL_DOWN,
	INPUT_PULL_UP,
	OUTPUT,
} pinmode_t;

void k210_gpio_set_high(uint8_t pin);

void k210_gpio_setup_mode_impl(uint8_t pin, pinmode_t mode){
	assert(pin < GPIO_NUM_PIN);
	uint8_t num_io = 24;

	// TODO
	assert(mode == OUTPUT);
	k210_fpioa_set_pull(num_io, FPIOA_PULL_DOWN);
	k210_gpio_set_dir(pin, 1);
}

// TODO: remove
void gpio_test(void){
	// turn on LED(24)
	// pin3 -> FPIOA[24]

	k210_fpioa_set_func(24, FN_GPIO3);
	k210_gpio_setup_mode_impl(3, OUTPUT);
	k210_gpio_set_high(3);
}

static int k210_gpio_init(void){
	//k210_fpioa_set_func(24, FN_GPIO3);
	// enable bus clock
	clk_en_cent->apb0 = 1;

	// enable device clock
	clk_en_peri->gpio = 1;

	gpio_test();
	return 0;
}

void k210_gpio_set_dir(uint8_t pin, uint8_t dir){
	volatile uint32_t *reg = gpio->dir.reg32;
	if(dir){
		*reg |= 1 << pin;
	}else{
		*reg &= ~(1 << pin);
	}
}

void k210_gpio_set_high(uint8_t pin){
	volatile uint32_t *reg = gpio->data_out.reg32;
	*reg |= 1 << pin;
}
