/**
 * @file
 * @brief Implements utilities functions for virtual console working.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 * @author Fedor Burdun
 */

#include <drivers/tty.h>
#include <drivers/tty_action.h>
#include <drivers/vconsole.h>
#include <fs/file.h>
#include <string.h>
#include <kernel/diag.h>
#include <embox/unit.h>

#if 0
#define _WITHOUT_CONSOLES
#endif

#if 0
#define HARD_DEBUG
#endif

static vconsole_t *def_console = NULL; /* &cur_tty->console[0]; */
//vconsole_t const *sys_console = &def_console;

vconsole_t *vconsole_open(tty_device_t *tty) {
	//return (vconsole_t *) sys_console;
	return (vconsole_t *) def_console;
}

void vconsole_putcharXY( struct vconsole *vc, uint8_t x, uint8_t y, char ch ) {
	vc->scr_buff[vc->width*y+x]=ch;
	return;
}

char vconsole_getchar(struct vconsole *vc) {
	return 0;
}

void vconsole_gotoxy(struct vconsole *vc, uint8_t x, uint8_t y) {
	vc->scr_column = x;
	vc->scr_line = y;
	/* say anythink for tty */
}

/* decouple this from tty_clear */
void vconsole_clear(struct vconsole *vc) {
	int32_t i;
	if (vc == NULL ) return;
	i = vc->width * vc->height - 1;
	while (vc->out_busy);
	for (; i >= 0; --i) {
		vc->scr_buff[i] = (uint8_t) ' ';
	}
	vc->scr_column = 0;
	vc->scr_line = 0;
}


static int console_init(struct vconsole *cons, int id, tty_device_t * tty) {
	cons->id = id;
	cons->tty = tty;
	cons->width = 80;
	cons->height = 25;
	cons->out_busy = false;
	return 0;
}

vconsole_t *vconsole_create(int id, tty_device_t *tty) {
	vconsole_t *console;
	//FIXME tty here must be console alloc
	console = (struct vconsole *)&cur_tty->console[id];
	console_init(console, id, tty);
	return console;
}
