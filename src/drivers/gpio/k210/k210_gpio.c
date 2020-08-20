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

#include <drivers/gpio/gpio_driver.h>
#include <drivers/gpio/k210.h>
#include <drivers/gpio/k210/fpioa.h>

EMBOX_UNIT_INIT(k210_gpio_init);

#define K210_GPIO_CHIP_ID OPTION_GET(NUMBER,gpio_chip_id)

volatile k210_gpio_t* const gpio = (volatile k210_gpio_t*) K210_GPIO_BASE_ADDR;

extern volatile sysctl_clock_enable_central* const clk_en_cent;
extern volatile sysctl_clock_enable_peripheral* const clk_en_peri;

static struct gpio_chip k210_gpio_chip = {
	.setup_mode = k210_gpio_setup_mode,
	.get = k210_gpio_get,
	.set = k210_gpio_set,
	.nports = K210_GPIO_PORTS_COUNT
};

static int k210_gpio_setup_mode(unsigned char port, gpio_mask_t pins, int mode){
	assert(port < K210_GPIO_PORTS_COUNT);

	if(mode & GPIO_MODE_OUTPUT){
		k210_gpio_set_dir(pins, 1);
	}else if(mode & GPIO_MODE_INPUT){
		log_error("GPIO input mode is not implemented");
		return -1;
	}else{
		log_error("GPIO mode %x is not implemented", mode);
		return -1;
	}

	return 0;
}

static void k210_gpio_set(unsigned char port, gpio_mask_t pins, char level){
	volatile uint32_t *reg_dir = gpio->dir.reg32;
	volatile uint32_t *reg = gpio->data_out.reg32;
	assert(port < K210_GPIO_PORTS_COUNT);

	// direction check
	uint32_t input = ~(*reg_dir);
	input = (uint32_t)pins & input;
	if(input){
		log_error("input pin cannot set level: pin mask=%x, level=%d", input, level);
	}

	if(level == GPIO_PIN_HIGH){
		*reg |= (uint32_t)pins;
		//log_debug("gpio_set high: %x", pins);
	}else if(level == GPIO_PIN_LOW){
		*reg &= ~((uint32_t)pins);
		//log_debug("gpio_set low: %x", pins);
	}else{
		log_error("unknown GPIO pin level");
	}
}

static gpio_mask_t k210_gpio_get(unsigned char port, gpio_mask_t pins){
	volatile uint32_t *reg_dir = gpio->dir.reg32;
	gpio_mask_t res = 0;
	assert(port < K210_GPIO_PORTS_COUNT);

	uint32_t dir = *reg_dir;
	int bit;
	bit_foreach(bit, (uint32_t)pins){
		volatile uint32_t *reg =
			(dir << bit ? gpio->data_out.reg32 : gpio->data_in.reg32);
		res |= *reg & (1 << bit);
	}
	return res;
}

// TODO: remove
void gpio_test(void){
	// turn on LED(24)
	// pin3 -> FPIOA[24]

	k210_fpioa_set_func(24, FN_GPIO3);
	k210_fpioa_set_pull(24, FPIOA_PULL_DOWN);
}

static int k210_gpio_init(void){
	//k210_fpioa_set_func(24, FN_GPIO3);
	// enable bus clock
	clk_en_cent->apb0 = 1;

	// enable device clock
	clk_en_peri->gpio = 1;

	gpio_test();

	return gpio_register_chip(&k210_gpio_chip, K210_GPIO_CHIP_ID);
}

void k210_gpio_set_dir(gpio_mask_t pins, bool dir){
	volatile uint32_t *reg = gpio->dir.reg32;
	if(dir){
		*reg |= (uint32_t)pins;
	}else{
		*reg &= ~((uint32_t)pins);
	}
}
