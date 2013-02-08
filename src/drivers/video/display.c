/**
 * @file
 *
 * @date 14.12.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/video/display.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <util/list.h>

static LIST_DEF(registered_displays);

void display_init(struct display *displ, const char *name,
		const struct display_options *ops, void *vga_regs) {
	memset(displ, 0, sizeof *displ);
	displ->name = name;
	list_link_init(&displ->lnk);
	displ->ops = ops;
	displ->vga_regs = vga_regs;
}

int display_register(struct display *displ) {
	assert(displ != NULL);
	if (!list_alone_element(displ, lnk)) {
		return -EEXIST;
	}
	list_add_last_element(displ, &registered_displays, lnk);
	return 0;
}

void display_unregister(struct display *displ) {
	assert(displ != NULL);
	assert(!list_alone_element(displ, lnk));

	list_unlink_element(displ, lnk);
}

struct display * display_get_default(void) {
	return list_first_element(&registered_displays, struct display, lnk);
}

void display_clear_screen(struct display *displ) {
	unsigned int x, y;

	assert(displ != NULL);
	assert(displ->ops != NULL);
	assert(displ->ops->set_pixel != NULL);

	/* clear screen */
	for (y = 0; y < displ->height; y++) {
		for (x = 0; x < displ->width; x++) {
			displ->ops->set_pixel(displ, x, y, 0);
		}
	}
}

void display_set_pixel(struct display *displ, unsigned int x, unsigned int y,
		unsigned int color) {
	assert(displ != NULL);
	assert(displ->ops != NULL);
	assert(displ->ops->set_pixel != NULL);
	displ->ops->set_pixel(displ, x, y, color);
}

unsigned int display_get_pixel(struct display *displ, unsigned int x,
		unsigned int y) {
	assert(displ != NULL);
	assert(displ->ops != NULL);
	assert(displ->ops->get_pixel != NULL);
	return displ->ops->get_pixel(displ, x, y);
}
