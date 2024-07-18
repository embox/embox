/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>

#include <drivers/gpio/gpio_driver.h>
#include <hal/reg.h>
#include <kernel/irq.h>

//#include <config/board_config.h>

#include <framework/mod/options.h>

struct pad_config_regs {
	volatile uint32_t PORT_0_CFG;
	volatile uint32_t PORT_0_DS;
	volatile uint32_t PORT_0_PUPD;

	volatile uint32_t PORT_1_CFG;
	volatile uint32_t PORT_1_DS;
	volatile uint32_t PORT_1_PUPD;

	volatile uint32_t PORT_2_CFG;
	volatile uint32_t PORT_2_DS;
	volatile uint32_t PORT_2_PUPD;
};

#define CONF_PAD_CONFIG_REGION_BASE 0x00084000

#define PAD_CONFIG                 ((volatile struct pad_config_regs *)CONF_GPIO_PORT_0_REGION_BASE)

