/**
 * @file
 * @brief
 *
 * @date 09.09.11
 * @author Anton Kozlov
 * @author Ilia Vaprol
 */

#include <drivers/diag.h>
#include <drivers/video_term.h>
#include <drivers/iodev.h>

void diag_putc(char ch) {
	vterm_putc(&diag_vterm, ch);
}

void diag_init(void) {
	iodev_diag_ops->init();
}
