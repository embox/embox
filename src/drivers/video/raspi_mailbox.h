/**
 * @file
 * @brief Mailbox Communication mechanism header for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#ifndef RASPI_MAILBOX_H_
#define RASPI_MAILBOX_H_

#include <stdint.h>

#define RASPI_FB_MAX_RES    4096
#define RASPI_FB_MAX_BPP    32

/**
 * The Mailbox gives the ability to send/receive messages to/from the processor
 * and the graphic card.
 */
#define BCM2835_MAILBOX_BASE            0x2000B880
#define BCM2835_MAILBOX_DATA_MASK       0xFFFFFFF0
#define BCM2835_MAILBOX_CHANNEL_MASK    0x0000000F
/* BCM2835 Mailbox Status Flags */
#define BCM2835_MAILBOX_EMPTY           0x40000000
#define BCM2835_MAILBOX_FULL            0x80000000

#define BCM2835_FRAMEBUFFER_CHANNEL     1

/**
 * Layout of the Mailbox Registers.
 */
struct raspi_mailbox_regs {
	uint32_t Read;              /* Receiving mail.				R- */
	uint32_t unused1;
	uint32_t unused2;
	uint32_t unused3;
	uint32_t Poll;              /* Receive without retrieving.	R- */
	uint32_t Sender;            /* Sender information.			R- */
	uint32_t Status;            /* Information.					R- */
	uint32_t Configuration;     /* Settings.					RW */
	uint32_t Write;             /* Sending mail.				-W */
};

/**
 * Format of the messages to the graphics processor.
 */
struct raspi_fb_info {
	uint32_t width_p;       /* width of the physical display */
	uint32_t height_p;      /* height of the physical display */
	uint32_t width_v;       /* width of the virtual display */
	uint32_t height_v;      /* height of the virtual display */
	uint32_t gpu_pitch;     /* 0 upon request; in response: num bytes between rows */
	uint32_t bit_depth;     /* number of bits to allocate in each pixel */
	uint32_t x;             /* x offset */
	uint32_t y;             /* y offset */
	uint32_t gpu_pointer;   /* actual pointer to the frame buffer */
	uint32_t gpu_size;      /* size of the frame buffer in bytes */
} __attribute__((aligned(16)));

/**
 * Prototypes
 */
int mailbox_write(uint32_t data, uint32_t channel);
uint32_t mailbox_read(uint32_t channel);

#endif	/* RASPI_MAILBOX_H_ */
