/**
 * @file k210_gpio.h
 * @brief
 * @author sksat <sksat@sksat.net>
 * @version 0.1
 * @date 2020-08-15
 */

#ifndef SRC_DRIVERS_GPIO_K210_GPIO_H_
#define SRC_DRIVERS_GPIO_K210_GPIO_H_

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpio.h>

// #define K210_GPIO_BASE_ADDR		(0x50200000U)
// #define K210_GPIO_PORTS_COUNT	1
// #define K210_GPIO_PIN_COUNT		8

typedef union {
	uint32_t reg32[1];
	uint16_t reg16[2];
	uint8_t reg8[4];
} __attribute__((packed, aligned(4))) k210_gpio_reg_t;

typedef struct {
	k210_gpio_reg_t enable;
	k210_gpio_reg_t mask;
	k210_gpio_reg_t level;
	k210_gpio_reg_t porality;
	k210_gpio_reg_t status;
	k210_gpio_reg_t status_raw;
	k210_gpio_reg_t debounce;
	k210_gpio_reg_t clear;
} __attribute__((packed, aligned(4))) k210_gpio_interrupt_t;

typedef struct {
	k210_gpio_reg_t data_out;
	k210_gpio_reg_t dir;
	k210_gpio_reg_t src;
	uint32_t unused[9];
	k210_gpio_interrupt_t interrupt;
	k210_gpio_reg_t data_in;
	uint32_t unused1[3];
	k210_gpio_reg_t sync_level;
	k210_gpio_reg_t id_code;
	k210_gpio_reg_t interrupt_bothedge;
} __attribute__((packed, aligned(4))) k210_gpio_t;

static int k210_gpio_init(void);
static int k210_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode);
static void k210_gpio_set(unsigned int port, gpio_mask_t pins, int level);
static gpio_mask_t k210_gpio_get(unsigned int port, gpio_mask_t pins);

void k210_gpio_set_dir(gpio_mask_t pins, bool dir);

#endif
