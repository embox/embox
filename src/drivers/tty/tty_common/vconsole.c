/**
 * @file
 * @brief Implements utilities functions for virtual console working.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 * @author Fedor Burdun
 */

#include <embox/unit.h>
#include <util/pool.h>
#include <string.h>
#include <drivers/tty.h>
#include <drivers/tty_action.h>
#include <drivers/vconsole.h>
//#include <fs/file.h>

#include <kernel/diag.h>


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

#ifdef CONFIG_TTY_CONSOLE_COUNT

POOL_DEF(vconsole_pool, vconsole_t, CONFIG_TTY_CONSOLE_COUNT);

static int console_setup(struct vconsole *cons, int id, tty_device_t * tty) {
	cons->id = id;
	cons->tty = tty;
	cons->width = 80;
	cons->height = 25;
	cons->out_busy = false;
	return 0;
}

vconsole_t *vconsole_create(int id, tty_device_t *tty) {
	vconsole_t *console;
	console = pool_alloc(&vconsole_pool);
	console_setup(console, id, tty);
	tty->consoles[id] = console;
	//console = (struct vconsole *) __vconsole_pool_pool[sizeof(vconsole_t) * id];
	//console = (struct vconsole *)&cur_tty->console[id];
	//console_setup(console, id, tty);
//	memcpy(&tty->console[id], console, sizeof(vconsole_t));
//	console = &tty->console[id];
	return console;
}
#endif
