/**
 * @file
 * @brief Frame Buffer header for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#ifndef RASPI_FRAMEBUFFER_H_
#define RASPI_FRAMEBUFFER_H_

#include <stdint.h>

#define FB_MAX_DIM			4096
#define FB_MAX_BIT_DEPTH	32

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768
#define BIT_DEPTH		16		// High Colour

#define CHAR_WIDTH	8
#define CHAR_HEIGHT 12

/**
 * The Mailbox gives the ability to send/receive messages to/from the processor
 * and the graphic card.
 */
#define BCM2835_MAILBOX_BASE 			0x2000B880
#define BCM2835_MAILBOX_DATA_MASK		0xFFFFFFF0
#define BCM2835_MAILBOX_CHANNEL_MASK	0x0000000F
/* BCM2835 Mailbox Status Flags */
#define BCM2835_MAILBOX_EMPTY			0x40000000
#define BCM2835_MAILBOX_FULL			0x80000000

/**
 * Layout of the Mailbox Registers.
 */
struct raspi_mailbox_regs {
	uint32_t Read;				/* Receiving mail. 				R- */
	uint32_t unused1;
	uint32_t unused2;
	uint32_t unused3;
	uint32_t Poll;				/* Receive without retrieving. 	R- */
	uint32_t Sender;			/* Sender information. 			R- */
	uint32_t Status;			/* Information.  				R- */
	uint32_t Configuration;		/* Settings. 					RW */
	uint32_t Write;				/* Sending mail.				-W */
};

/**
 * Format of the messages to the graphics processor.
 */
struct frame_buffer_info {
	uint32_t width_p;	/* width of the physical display */
	uint32_t height_p;	/* height of the physical display */
	uint32_t width_v;	/* width of the virtual display */
	uint32_t height_v;	/* height of the virtual display */
	uint32_t gpu_pitch;	/* 0 upon request; in response: num bytes between rows*/
	uint32_t bit_depth;	/* number of bits to allocate in each pixel */
	uint32_t x;			/* x offset */
	uint32_t y;			/* y offset */
	uint32_t gpu_pointer;	/* actual pointer to the frame buffer */
	uint32_t gpu_size;		/* size of the frame buffer in bytes */
} __attribute__((aligned(16)));

extern struct frame_buffer_info fb_info;

/**
 * Prototypes
 */
int mailbox_write(uint32_t data, uint32_t channel);
uint32_t mailbox_read(uint32_t channel);
struct frame_buffer_info * init_frame_buffer(uint32_t width, uint32_t height,
	uint32_t bit_depth);

/* Functions to draw things on the screen */
void set_pixel(uint32_t x, uint32_t y, uint32_t colour);
void draw_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t
		colour);
void draw_character(uint32_t x, uint32_t y, char c, uint32_t colour);
void raspi_putc(const char c, uint32_t colour);
void draw_string(const char *s, uint32_t colour);

#endif
