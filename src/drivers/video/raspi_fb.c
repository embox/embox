/**
 * @file
 * @brief Frame Buffer implementation for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <errno.h>
#include <drivers/video/raspi_fb.h>
#include <defines/null.h>

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

/**
 * Initialise a the frame buffer passed as parameter.
 * Input:
 * - width, of the display (must be lower than RASPI_FB_MAX_RES)
 * - height, of the display (must be lower than RASPI_FB_MAX_RES)
 * - bit_depth, number of bits to allocate in each pixel
 * - fb_info, pointer to a frame buffer information structure
 * Output:
 * - 0 on success
 * - error code, otherwise
 */
int init_raspi_fb(uint32_t width, uint32_t height, uint32_t bit_depth,
		struct raspi_fb_info *fb_info) {
	/* Validate Inputs */
	if (width > RASPI_FB_MAX_RES || height > RASPI_FB_MAX_RES ||
		bit_depth > RASPI_FB_MAX_BPP) {
		return -EINVAL;
	}

	/* Write inputs into the frame buffer */
	fb_info->width_p        = width;
	fb_info->height_p       = height;
	fb_info->width_v        = width;
	fb_info->height_v       = height;
	fb_info->gpu_pitch      = 0;
	fb_info->bit_depth      = bit_depth;
	fb_info->x              = 0;
	fb_info->y              = 0;
	fb_info->gpu_pointer    = 0;
	fb_info->gpu_size       = 0;

	/**
	 * Send the address of the frame buffer + 0x40000000 to the mailbox
	 *
	 * By adding 0x40000000, we tell the GPU not to use its cache for these
	 * writes, which ensures we will be able to see the change
	 */
	mailbox_write(((uint32_t) fb_info) + 0x40000000, BCM2835_FRAMEBUFFER_CHANNEL);
	if (mailbox_read(BCM2835_FRAMEBUFFER_CHANNEL) != 0 ||
			!fb_info->gpu_pointer) {
		return -EAGAIN;
	}

	return 0;
}
