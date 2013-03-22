/*
 * @file
 *
 * @date Mar 18, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <drivers/iodev.h>
#include <drivers/video_term.h>
#include <drivers/iodev_diag.h>

struct vterm diag_vterm;
struct tty *diag_tty = &diag_vterm.tty;

int diag_console_init(void) {
	vterm_init(&diag_vterm, &diag_vga.video, NULL);
	iodev_setup(iodev_diag_ops);
	iodev_init();
	return 0;
}
