/**
 * @file
 * @brief Mailbox Communication mechanism for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <errno.h>
#include <stdint.h>

#include <drivers/video/raspi_mailbox.h>

static volatile struct raspi_mailbox_regs *const mailbox_regs =
		(volatile struct raspi_mailbox_regs *) BCM2835_MAILBOX_BASE;

static inline void data_mem_barrier(void) {
	__asm__ __volatile__(
		"mov r12, #0; \n\t"
		"mcr p15, 0, r12, c7, c10, 5; \n\t"
	);
}

/**
 * To omit illegible lines of code, a helper function that reads from
 * memory mapped IO registers.
 */
uint32_t readMMIO(volatile uint32_t *addr)
{
	uint32_t n;

	data_mem_barrier();
	n = *addr;
	data_mem_barrier();
	return n;
}

/**
 * The opposite of above. Write to MMIO.
 */
void writeMMIO(volatile uint32_t *addr, uint32_t val)
{
	data_mem_barrier();
	*addr = val;
	data_mem_barrier();
}

/**
 * Routine to send a message to a particular mailbox.
 * Input:
 * - data, message to be sent
 * - channel, what mailbox to write the message to
 * Output:
 * - 0, in case of success
 * - <0, in case of error
 */
int mailbox_write(uint32_t data, uint32_t channel) {
	/* validate channel number */
	if (channel > 15) {
		return -EINVAL;
	}
	/* lowest 4 bits of data must be 0 and combine data and the channel */
	uint32_t to_write = (data & BCM2835_MAILBOX_DATA_MASK) | channel;
	/* read the status field and wait until ready */
	while (readMMIO((volatile uint32_t *)&mailbox_regs->Status) & BCM2835_MAILBOX_FULL);
	writeMMIO((volatile uint32_t *)&mailbox_regs->Write, to_write);
	return 0;
}

/**
 * Routine to read a message from a particular mailbox.
 * Input:
 * - channel, what mailbox to receive the message from
 * Output:
 * - the data read in case of success
 * - <0, in case of error
 */
uint32_t mailbox_read(uint32_t channel) {
	uint32_t data;
	
	if (channel > 15) {
		return -EINVAL;
	}

	data = 0;
	/* repeat reading if the channel was wrong */
	while ((data & BCM2835_MAILBOX_CHANNEL_MASK) != channel) {
		while (readMMIO((volatile uint32_t *)&mailbox_regs->Status) & BCM2835_MAILBOX_EMPTY);
		data = readMMIO((volatile uint32_t *)&mailbox_regs->Read);
	}
	/* return the message only (top 28 bits of the read data) */
	return data & BCM2835_MAILBOX_DATA_MASK;
}
