/**
 * @file
 * @brief Implements utilities functions for virtual console working.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 */

#include <drivers/tty.h>
#include <drivers/tty_action.h>
#include <drivers/vconsole.h>
#include <fs/file.h>
#include <string.h>
#include <kernel/diag.h>
#include <embox/unit.h>

static vconsole_t def_console;
vconsole_t *cur_console = NULL;
vconsole_t const *sys_console = &def_console;

//EMBOX_UNIT_INIT(vconsole_init);

static FILE *def_file;

static int vconsole_init(void) {
	/*initialize pool of virtual consoles*/
	/*initialize default console*/
	//def_file = fopen(CONFIG_DEFAULT_CONSOLE,"r"); /* was moved to tty.c */
	//def_console.tty = cur_tty;
	return 0;
}

vconsole_t *vconsole_open(tty_device_t *tty) {
	return (vconsole_t *) sys_console;
}

void vconsole_loadline(vconsole_t *con) {
	uint32_t *s = &con->tty->rx_cnt;
	uint32_t *t = &con->tty->rx_cur;
	/* write saved command line */
	*t = con->cl_cur;
	for (*s=0; *s<con->cl_cnt; ++*s) {
		diag_putc(con->tty->rx_buff[*s] = con->cl_buff[*s]);
		//con->tty->file_op->fwrite()
	}
	/* go to saved cursor position */
	for (;*s>*t;--*s) {
		vtbuild(con->tty->vtb, TOKEN_LEFT);
	}
	*s = con->cl_cnt;
}

void vconsole_saveline(vconsole_t *con) {
	uint32_t *s, *t;
	s = &con->tty->rx_cnt;
	t = &con->tty->rx_cur;
	con->cl_cnt = *s;
	con->cl_cur = *t;
	/* clear current command line */
	for (;*t>0;--*t) {
		vtbuild(con->tty->vtb, TOKEN_LEFT);
	}
	for (;*t<*s;++*t) {
		con->tty->file_op->fwrite(" ",sizeof(char),1,NULL);
	}
	/* copy command line in buffer */
	con->cl_buff[0] = con->tty->rx_buff[0];
	for (;*s>0;--*s) {
		con->cl_buff[*s] = con->tty->rx_buff[*s];
		vtbuild(con->tty->vtb, TOKEN_LEFT);
	}
	*s = *t = 0;
}

int vconsole_activate(vconsole_t *con) {
	vconsole_reprint(con);
	vconsole_loadline(con);
	return 0;
}

int vconsole_deactivate(vconsole_t *con) {
	vconsole_saveline( con );
	return 0;
}

int vconsole_close(vconsole_t *con) {
	return 0;
}

#ifdef CONFIG_TTY_CONSOLE_COUNT
inline bool its_cur(void) {
	if (cur_console==NULL) {
		return false;
	}
	return &cur_console->tty->console[cur_console->tty->console_cur] == cur_console;
}
#define ITS_CUR	its_cur()
#define ICC(a) do { if (its_cur()) a; } while (0)
#else
#define ITS_CUR	true
#define ICC(a) a
#endif

void vconsole_putchar( struct vconsole *vc, char ch ) {
	/* FIXME */
	diag_putc( ch );
	return;

	if (vc==NULL) { /* if hasn't initialized now cur_console use hardware output */
		diag_putc( ch );
		return;
	}

	printk("test %c\n",ch);
	return;

	if (vc->scr_column >= vc->width) {
		ICC(vc->tty->file_op->fwrite("\n", sizeof(char), 1, NULL));
		++ vc->scr_line;
		vc->scr_column = 0;
	}
	if (vc->scr_line >= vc->height) {
		uint32_t i = vc->width * (vc->height - 1);
		memcpy( &vc->scr_buff[vc->width] , &vc->scr_buff[0] , i * sizeof(char));
		for (; i<(vc->width*vc->height); ++i ) {
			vc->scr_buff[i] = ' ';
		}
		ICC( vconsole_reprint( vc ) );
		vc->scr_line = vc->height;
	}
	vc->scr_buff[ (vc->scr_line * vc->width) + vc->scr_column ] = ch;
	ICC( vc->tty->file_op->fwrite(&ch, sizeof(char), 1, NULL) );
}

char vconsole_getchar(struct vconsole *vc) {
	return 0;
}

void vconsole_gotoxye(struct vconsole *vc, uint8_t lx, uint8_t ly, uint8_t nx, uint8_t ny) {
	if (ITS_CUR) {
		uint8_t i;
		if (lx<nx) {
			for (i=0;i<(nx-lx);++i) {
				vtbuild(vc->tty->vtb, TOKEN_RIGHT);
			}
		} else {
			for (i=0;i<(lx-nx);++i) {
				vtbuild(vc->tty->vtb, TOKEN_LEFT);
			}
		}
		if (ly<ny) {
			for (i=0;i<(ny-ly);++i) {
				vtbuild(vc->tty->vtb, TOKEN_DOWN);
			}
		} else {
			for (i=0;i<(ly-ny);++i) {
				vtbuild(vc->tty->vtb, TOKEN_UP);
			}
		}
	}
	vc->scr_column = nx;
	vc->scr_line = ny;
}

void vconsole_gotoxy( struct vconsole *vc, uint8_t x, uint8_t y ) {
	vconsole_gotoxye( vc , vc->scr_column, vc->scr_line, x, y);
}

void vconsole_clear(struct vconsole *vc) {
	return;
	uint32_t i = vc->width * vc->height - 1;
	vc->tty->file_op->fwrite("\n", sizeof(char), 1, NULL);
	for (; i >= 0; --i) {
		ICC(vconsole_putchar( vc , ' ' ));
		vc->scr_buff[i] = ' ';
	}
	vconsole_gotoxye(vc, vc->width, vc->height, 0, 0);
}

void vconsole_reprint(struct vconsole *vc) {
	return;
	size_t i;
	for (i = 0; i < (vc->width * vc->height); ++i) {
		vconsole_putchar(vc, vc->scr_buff[i]);
	}
	vconsole_gotoxy(vc, vc->scr_column, vc->scr_line);
}

