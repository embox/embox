/**
 * @file
 * @brief Mailbox Communication mechanism for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <errno.h>
#include <stdint.h>

#include <drivers/mailbox/bcm2835_mailbox.h>
#include <hal/reg.h>
#include <hal/mem_barriers.h>

static struct bcm2835_mailbox_regs volatile *const mailbox_regs =
		(struct bcm2835_mailbox_regs volatile *) BCM2835_MAILBOX_BASE;

/**
 * Read from memory mapped IO registers with a memory barrier around.
 * @param addr - pointer to register address
 *
 * @return data read
 */
static uint32_t read_mmio(uint32_t volatile *addr) {
	uint32_t n;
	n = REG_LOAD(addr);
	data_mem_barrier();
	return n;
}

/**
 * Write to memory mapped IO register with a memory barrier around.
 * @param addr - pointer to register address
 * @param val - data to be stored
 */
static void write_mmio(uint32_t volatile *addr, uint32_t val) {
	data_mem_barrier();
	REG_STORE(addr, val);
}

int bcm2835_mailbox_write(uint32_t data, uint32_t channel) {
	/* validate channel number */
	if (channel > 15) {
		return -EINVAL;
	}
	/* lowest 4 bits of data must be 0 and combine data and the channel */
	uint32_t to_write = (data & BCM2835_MAILBOX_DATA_MASK) | channel;
	/* read the status field and wait until ready */
	while (read_mmio((uint32_t volatile *)&mailbox_regs->Status) & BCM2835_MAILBOX_FULL);
	write_mmio((uint32_t volatile *)&mailbox_regs->Write, to_write);
	return 0;
}

uint32_t bcm2835_mailbox_read(uint32_t channel) {
	uint32_t data;

	if (channel > 15) {
		return -EINVAL;
	}

	/* repeat reading if the channel was wrong */
	do {
		while (read_mmio((uint32_t volatile *)&mailbox_regs->Status) & BCM2835_MAILBOX_EMPTY);
		data = read_mmio((uint32_t volatile *)&mailbox_regs->Read);
	} while ((data & BCM2835_MAILBOX_CHANNEL_MASK) != channel);
	/* return the message only (top 28 bits of the read data) */
	return data & BCM2835_MAILBOX_DATA_MASK;
}
