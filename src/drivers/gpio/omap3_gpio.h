/**
 * @file
 * @brief OMAP3 GPIO driver definitions
 *
 * @date 1.11.13
 * @author Alexander Kalmuk
 */

#ifndef OMAP3_GPIO_H_
#define OMAP3_GPIO_H_

#include <stdint.h>
#include <hal/reg.h>
#include <kernel/irq.h>

typedef volatile unsigned long __gpio_mask_t;

#define N_PINS 8 * (sizeof(int))
#define GPIO_REG_SIZE 0x2000

struct omap_pin_handler {
	int mask;
	void *data;
	irq_handler_t handler;
};

struct gpio {
	unsigned long base;
	int gpio_nr;
	struct omap_pin_handler pin[N_PINS];
};

#define GPIO_BASE_ADDRESS(i)      ((i) == 1 ? 0x48310000 : (0x49050000 + ((i) - 2) * GPIO_REG_SIZE))
#define GPIO_IRQ(i)               (28 + (i))
#define GPIO_NUM_BY_IRQ(i)        ((i) - 28)
#define GPIO_MODULE_CNT           6


/* Register offsets from base address */
#define GPIO_REVISION             0x00
#define GPIO_SYSCONFIG            0x10
#define GPIO_SYSSTATUS            0x14
#define GPIO_IRQSTATUS1           0x18
#define GPIO_IRQENABLE1           0x1C
#define GPIO_WAKEUPENABLE         0x20
#define GPIO_IRQSTATUS2           0x28
#define GPIO_IRQENABLE2           0x2C
#define GPIO_CTRL                 0x30
#define GPIO_OE                   0x34
#define GPIO_DATAIN               0x38
#define GPIO_DATAOUT              0x3C
#define GPIO_LEVELDETECT0         0x40
#define GPIO_LEVELDETECT1         0x44
#define GPIO_RISINGDETECT         0x48
#define GPIO_FALLINGDETECT        0x4C
#define GPIO_DEBOUNCENABLE        0x50
#define GPIO_DEBOUNCINGTIME       0x54
#define GPIO_CLEARIRQENABLE1      0x60
#define GPIO_SETIRQENABLE1        0x64
#define GPIO_CLEARIRQENABLE2      0x70
#define GPIO_SETIRQENABLE2        0x74
#define GPIO_CLEARWKUENA          0x80
#define GPIO_SETWKUENA            0x84
#define GPIO_CLEARDATAOUT         0x90
#define GPIO_SETDATAOUT           0x94

#define GPIO_REVISION_MAJOR(l)    ((l >> 4) & 0xF)
#define GPIO_REVISION_MINOR(l)    (l & 0xF)

static inline uint32_t gpio_reg_read(unsigned long base, int offset) {
	unsigned long reg_addr =  (base + (unsigned long) offset);
	return REG_LOAD(reg_addr);
}

static inline void gpio_reg_write(unsigned long base, int offset, uint32_t val) {
	unsigned long reg_addr = (base + (unsigned long) offset);
	REG_STORE(reg_addr, val);
}

#endif /* OMAP3_GPIO_H_ */
