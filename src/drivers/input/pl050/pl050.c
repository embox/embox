/**
 * @file
 *
 * @date Apr 30, 2021
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <drivers/pl050.h>

#define PL050_CR_ENABLE     0x04
#define PL050_CR_RXINT_EN   0x10 /* 1<<4 */

#define PL050_STAT_TXBUSY   0x20 /* 1<<5 */

int pl050_init(struct pl050 *dev) {
	volatile uint32_t ack;
	dev->cr = PL050_CR_ENABLE;

	dev->data = 0xF4;
	while(dev->stat & PL050_STAT_TXBUSY) {
	}
	ack = dev->data;

	log_debug("%x", ack);

	dev->cr = PL050_CR_ENABLE | PL050_CR_RXINT_EN;

	return 0;
}
