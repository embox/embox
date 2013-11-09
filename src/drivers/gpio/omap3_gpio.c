/**
 * @file
 * @brief OMAP3 GPIO driver
 *
 * @date 1.11.13
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <hal/pins.h>
#include "omap3_gpio.h"

EMBOX_UNIT_INIT(gpio_init);

static int gpio_init(void) {
	uint32_t rev;

	rev = gpio_reg_read(6, GPIO_REVISION);

	printk("maj = %d, min = %d\n", GPIO_REVISION_MAJOR(rev), GPIO_REVISION_MINOR(rev));

#if 0
	for (i = 0; i < GPIO_MODULE_CNT; i++) {
		irq_attach(GPIO_IRQ(i), irq_pin_handler, 0, NULL, "OMAP3 GPIO pins");
		gpio_reg_write(i, GPIO_IRQENABLE1, mask);
	}
#endif

	return irq_attach(GPIO_IRQ(6), irq_pin_handler, 0, NULL, "OMAP3 GPIO6 pins");
}
