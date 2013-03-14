/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.03.2013
 */

#ifndef CONSOLE_FBCON_H_
#define CONSOLE_FBCON_H_

#include <kernel/thread/event.h>
#include <drivers/video/vesa_modes.h>
#include <drivers/console/mpx.h>
#include <drivers/tty.h>

#define FBCON_INPB 16

struct fbcon {
	struct vc vc_this;
	struct tty tty_this;

	struct video_resbpp resbpp;

	struct event inpevent;

	volatile char hasc;
	volatile char ascii[FBCON_INPB];
};

extern volatile struct fbcon *vc_fbcon_cur;

extern int iodev_vc_init(struct tty *tty);

#endif /* CONSOLE_FBCON_H_ */

