/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    17.07.2024
 */

#include <stdint.h>

#include <drivers/gpio.h>
#include <hal/reg.h>
#include <kernel/irq.h>

//#include <config/board_config.h>

#include <framework/mod/options.h>

struct pad_config_port_regs {
	volatile uint32_t PORT_CFG;
	volatile uint32_t PORT_DS;
	volatile uint32_t PORT_PUPD;
};

struct pad_config_regs {
	struct pad_config_port_regs pad_config_port_regs[3];
};

#define CONF_PAD_CONFIG_REGION_BASE 0x000050c00

#define PAD_CONFIG \
	((volatile struct pad_config_regs *)CONF_PAD_CONFIG_REGION_BASE)

void mik_pad_cfg_set_func(int port, int pin, int func) {
	volatile struct pad_config_port_regs *port_regs;
	uint32_t mask;

	port_regs = &(PAD_CONFIG->pad_config_port_regs[port]);

	mask = 0x3 << (pin << 1);
	port_regs->PORT_CFG &= ~mask;
	mask = func << (pin << 1);
	port_regs->PORT_CFG |= mask;
}

void mik_pad_cfg_set_ds(int port, int pin, int ds) {
	volatile struct pad_config_port_regs *port_regs;
	uint32_t mask;

	port_regs = &(PAD_CONFIG->pad_config_port_regs[port]);

	mask = 0x3 << (pin << 1);
	port_regs->PORT_DS &= ~mask;
	mask = ds << (pin << 1);
	port_regs->PORT_DS |= mask;
}

void mik_pad_cfg_set_pupd(int port, int pin, int pupd) {
	volatile struct pad_config_port_regs *port_regs;
	uint32_t mask;

	port_regs = &(PAD_CONFIG->pad_config_port_regs[port]);

	mask = 0x3 << (pin << 1);
	port_regs->PORT_PUPD &= ~mask;
	mask = pupd << (pin << 1);
	port_regs->PORT_PUPD |= mask;
}
