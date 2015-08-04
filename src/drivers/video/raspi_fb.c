/**
 * @file
 * @brief Frame Buffer implementation for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <drivers/video/raspi_fb.h>
#include <defines/null.h>

static volatile struct raspi_mailbox_regs *const mailbox_regs =
		(volatile struct raspi_mailbox_regs *) BCM2835_MAILBOX_BASE;

static inline void data_mem_barrier(void) {
	__asm__ __volatile__("mov r12, #0; \n\t"
						 "mcr p15, 0, r12, c7, c10, 5; \n\t");
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
	if (channel > 15)
		return -1;
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
	
	if (channel > 15)
		return -1;

	data = 0;
	/* repeat reading if the channel was wrong */
	while ((data & BCM2835_MAILBOX_CHANNEL_MASK) != channel) {
		while (readMMIO((volatile uint32_t *)&mailbox_regs->Status) & BCM2835_MAILBOX_EMPTY);
		data = readMMIO((volatile uint32_t *)&mailbox_regs->Read);
	}
	/* return the message only (top 28 bits of the read data) */
	return data & BCM2835_MAILBOX_DATA_MASK;
}

struct frame_buffer_info fb_info;

/**
 * Initialise a new frame buffer and return its pointer to the caller.
 * Input:
 * - width, of the display (must be lower than FB_MAX_DIM)
 * - height, of the display (must be lower than FB_MAX_DIM)
 * - bit_depth, number of bits to allocate in each pixel
 * Output:
 * - pointer to the frame buffer
 */
struct frame_buffer_info * init_frame_buffer(uint32_t width, uint32_t height,
	   							uint32_t bit_depth) {
	/* Validate Inputs */
	if (width > FB_MAX_DIM || height > FB_MAX_DIM ||
		   bit_depth > FB_MAX_BIT_DEPTH)
		return NULL;

	/* Write inputs into the frame buffer */
	fb_info.width_p = width;
	fb_info.height_p = height;
	fb_info.width_v = width;
	fb_info.height_v = height;
	fb_info.gpu_pitch = 0;
	fb_info.bit_depth = bit_depth;
	fb_info.x = 0;
	fb_info.y = 0;
	fb_info.gpu_pointer = 0;
	fb_info.gpu_size = 0;

	/**
	 * Send the address of the frame buffer + 0x40000000 to the mailbox
	 *
	 * By adding 0x40000000, we tell the GPU not to use its cache for these
	 * writes, which ensures we will be able to see the change
	 */
	mailbox_write(((uint32_t) &fb_info) + 0x40000000, 1);
	if (mailbox_read(1) != 0 || !fb_info.gpu_pointer)
		return NULL;

	return &fb_info;
}

extern int rows, cols;
extern int cursor_row;
extern int cursor_col;

static unsigned char FONT[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*' '*/
0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, /*'!'*/
0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'"'*/
0x00, 0x00, 0x14, 0x14, 0x3e, 0x14, 0x3e, 0x14, 0x14, 0x00, 0x00, 0x00, /*'#'*/
0x00, 0x00, 0x08, 0x3c, 0x0a, 0x1c, 0x28, 0x1e, 0x08, 0x00, 0x00, 0x00, /*'$'*/
0x00, 0x00, 0x06, 0x26, 0x10, 0x08, 0x04, 0x32, 0x30, 0x00, 0x00, 0x00, /*'%'*/
0x00, 0x00, 0x1c, 0x02, 0x02, 0x04, 0x2a, 0x12, 0x2c, 0x00, 0x00, 0x00, /*'&'*/
0x00, 0x18, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'''*/
0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x00, /*'('*/
0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x02, 0x00, /*')'*/
0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, /*'*'*/
0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, /*'+'*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, /*','*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'-'*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, /*'.'*/
0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x00, 0x00, /*'/'*/
0x00, 0x1c, 0x22, 0x32, 0x2a, 0x26, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'0'*/
0x00, 0x08, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /*'1'*/
0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'2'*/
0x00, 0x1c, 0x22, 0x20, 0x18, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'3'*/
0x00, 0x10, 0x18, 0x18, 0x14, 0x14, 0x3e, 0x10, 0x38, 0x00, 0x00, 0x00, /*'4'*/
0x00, 0x3e, 0x02, 0x02, 0x1e, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'5'*/
0x00, 0x18, 0x04, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'6'*/
0x00, 0x3e, 0x22, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x00, 0x00, 0x00, /*'7'*/
0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'8'*/
0x00, 0x1c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x10, 0x0c, 0x00, 0x00, 0x00, /*'9'*/
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, /*':'*/
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, /*';'*/
0x00, 0x00, 0x00, 0x30, 0x0c, 0x03, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, /*'<'*/
0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, /*'='*/
0x00, 0x00, 0x00, 0x03, 0x0c, 0x30, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, /*'>'*/
0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, /*'?'*/
0x00, 0x00, 0x1c, 0x22, 0x3a, 0x3a, 0x1a, 0x02, 0x1c, 0x00, 0x00, 0x00, /*'@'*/
0x00, 0x00, 0x08, 0x14, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00, 0x00, /*'A'*/
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, /*'B'*/
0x00, 0x00, 0x1c, 0x22, 0x02, 0x02, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'C'*/
0x00, 0x00, 0x0e, 0x12, 0x22, 0x22, 0x22, 0x12, 0x0e, 0x00, 0x00, 0x00, /*'D'*/
0x00, 0x00, 0x3e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'E'*/
0x00, 0x00, 0x3e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /*'F'*/
0x00, 0x00, 0x1c, 0x22, 0x02, 0x32, 0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, /*'G'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'H'*/
0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00, 0x00, /*'I'*/
0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'J'*/
0x00, 0x00, 0x22, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x22, 0x00, 0x00, 0x00, /*'K'*/
0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'L'*/
0x00, 0x00, 0x22, 0x36, 0x2a, 0x2a, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'M'*/
0x00, 0x00, 0x22, 0x26, 0x26, 0x2a, 0x32, 0x32, 0x22, 0x00, 0x00, 0x00, /*'N'*/
0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'O'*/
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /*'P'*/
0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x30, 0x00, 0x00, /*'Q'*/
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x0a, 0x12, 0x22, 0x00, 0x00, 0x00, /*'R'*/
0x00, 0x00, 0x1c, 0x22, 0x02, 0x1c, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'S'*/
0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /*'T'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'U'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, /*'V'*/
0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x36, 0x22, 0x00, 0x00, 0x00, /*'W'*/
0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00, 0x00, /*'X'*/
0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /*'Y'*/
0x00, 0x00, 0x3e, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3e, 0x00, 0x00, 0x00, /*'Z'*/
0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, /*'['*/
0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00, /*'\'*/
0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00, /*']'*/
0x00, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'^'*/
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, /*'_'*/
0x00, 0x0c, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'`'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x32, 0x2c, 0x00, 0x00, 0x00, /*'a'*/
0x00, 0x02, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, /*'b'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x02, 0x02, 0x3c, 0x00, 0x00, 0x00, /*'c'*/
0x00, 0x20, 0x20, 0x20, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, /*'d'*/
0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x3e, 0x02, 0x1c, 0x00, 0x00, 0x00, /*'e'*/
0x00, 0x38, 0x04, 0x04, 0x1e, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, /*'f'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x1c, /*'g'*/
0x00, 0x02, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'h'*/
0x00, 0x08, 0x08, 0x00, 0x0c, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, /*'i'*/
0x00, 0x10, 0x10, 0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0e, /*'j'*/
0x00, 0x02, 0x02, 0x02, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x00, 0x00, 0x00, /*'k'*/
0x00, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, /*'l'*/
0x00, 0x00, 0x00, 0x00, 0x16, 0x2a, 0x2a, 0x2a, 0x22, 0x00, 0x00, 0x00, /*'m'*/
0x00, 0x00, 0x00, 0x00, 0x1a, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /*'n'*/
0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /*'o'*/
0x00, 0x00, 0x00, 0x00, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x02, /*'p'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x20, /*'q'*/
0x00, 0x00, 0x00, 0x00, 0x1a, 0x06, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /*'r'*/
0x00, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x1c, 0x20, 0x1e, 0x00, 0x00, 0x00, /*'s'*/
0x00, 0x08, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x30, 0x00, 0x00, 0x00, /*'t'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x32, 0x2c, 0x00, 0x00, 0x00, /*'u'*/
0x00, 0x00, 0x00, 0x00, 0x36, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, /*'v'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x2a, 0x2a, 0x2a, 0x14, 0x00, 0x00, 0x00, /*'w'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, /*'x'*/
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x1c, /*'y'*/
0x00, 0x00, 0x00, 0x00, 0x3e, 0x10, 0x08, 0x04, 0x3e, 0x00, 0x00, 0x00, /*'z'*/
0x20, 0x10, 0x10, 0x10, 0x10, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x00, /*'{'*/
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, /*'|'*/
0x02, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x04, 0x02, 0x00, /*'}'*/
0x00, 0x04, 0x2a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*'~'*/
0x00, 0x00, 0x00, 0x08, 0x08, 0x14, 0x14, 0x22, 0x3e, 0x00, 0x00, 0x00, /*DEL*/
};

/**
 *	Sets pixel at coordinates (x,y) with the specified colour.
 *	Input:
 *	- x, horizontal coordinate
 *	- y, vertical coordinate
 *	- colour
 *	Output:
 *	- none
 */
void set_pixel(uint32_t x, uint32_t y, uint32_t colour) {
	volatile uint32_t *addr;

	if (x < 0 || x > fb_info.width_p || y < 0 || y > fb_info.height_p)
		return;
	/**
	 * Compute the address of the pixel as fb_addr + (x + y*width)*pixel_size
	 * pixel size is 2
	 */
	addr = (uint32_t *) (fb_info.gpu_pointer + ((x + y*fb_info.width_p) << 1));
	*addr = colour;
}

/**
 * Draws a line between points (x0,y0) and (x1,y1) using the specified colour.
 * Input:
 * - (x0,y0), coordinates of the start point
 * - (x1,y1), coordinates of the end point
 * - colour
 * Output:
 * - none
 */
void draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t
		colour) {
	uint32_t deltax, deltay;
	int32_t stepx, stepy, error;

	if (x1 > x0) {
		deltax = x1 - x0;
		stepx = 1;
	} else {
		deltax = x0 - x1;
		stepx = -1;
	}
	if (y1 > y0) {
		deltay = y1 - y0;
		stepy = 1;
	} else {
		deltay = y0 - y1;
		stepy = -1;
	}

	error = ((int32_t)deltax) - ((int32_t)deltay);
	while ((x0 != (x1 + stepx)) && (y0 != (y1 + stepy))) {
		set_pixel(x0, y0, colour);
		if ((error << 1) >= -deltay) {
			x0 += stepx;
			error -= deltay;
		}
		if ((error << 1) <= deltax) {
			y0 += stepy;
			error -= deltax;
		}
	}
}

/**
 * Draw a character on the screen at the specified coordinates with the
 * specified colour.
 * Input:
 * - (x,y), point where to draw the character
 * - c, character to be drawn
 * - colour
 * Output:
 * - none
 */
void draw_character(uint32_t x, uint32_t y, char c, uint32_t colour) {
	unsigned char line;
	int row, bit;

	if (c < 0 || c > 127)
		return;

	for (row = 0; row < CHAR_HEIGHT; row++) {
		line = FONT[c*CHAR_HEIGHT + row];
		for (bit = 0; bit < CHAR_WIDTH; bit++) {
			if (line & 0x1)
				set_pixel(x + bit, y, colour);
			line >>= 1;
		}
		y++;
	}
}

/**
 * Standard library putc to draw a character on the Raspberry Pi screen.
 * Input:
 * - c, character to be drawn
 * - colour
 * Output:
 * - none
 */
void raspi_putc(const char c, uint32_t colour) {
	draw_character(cursor_col*CHAR_WIDTH, cursor_row*CHAR_HEIGHT, c, colour);
	if (c == '\n') {
		cursor_row++;
		cursor_col = 0;
	} else if (c == '\t')
		cursor_col += 4;
	else
		cursor_col++;

	if (cursor_col == cols) {
		cursor_row++;
		cursor_col = 0;
	} else if (cursor_row == rows) {
		//TODO: clear one line
		cursor_col = 0;
	}
}

/**
 * Draw a string on the screen with the specified colour.
 * Input:
 * - s, string to be displayed
 * - colour
 * Output:
 * - none
 */
void draw_string(const char *s, uint32_t colour) {
	while (*s != '\0') {
		raspi_putc(*s, colour);
		s++;
	}
}
