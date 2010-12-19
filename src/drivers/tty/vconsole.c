/**
 * @file
 *
 * @brief Implements utilities functions for virtual console working.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 */

#include <drivers/tty.h>
#include <drivers/tty_action.h>
#include <drivers/vconsole.h>
#include <fs/file.h>
#include <embox/unit.h>

static vconsole_t def_console;
vconsole_t *cur_console;
vconsole_t const *sys_console = &def_console;

EMBOX_UNIT_INIT(vconsole_init);

static FILE *def_file;
#define CONFIG_DEFAULT_CONSOLE "/dev/uart"


static int vconsole_init(void) {
	/*initialize pool of virtual consoles*/
	/*initialize default console*/
	def_file = fopen(CONFIG_DEFAULT_CONSOLE,"r");
	def_console.tty = cur_tty;
	return 0;
}

vconsole_t *vconsole_open(tty_device_t *tty) {
	return (vconsole_t *) sys_console;
}

int vconsole_activate(vconsole_t *con) {
	uint32_t *s = &con->tty->rx_cnt;
	uint32_t *t = &con->tty->rx_cur;
	/* write saved command line */
	*t = con->cl_cur;
	for (*s=0; *s<con->cl_cnt; ++*s) {
		uart_putc( con->tty->rx_buff[*s] = con->cl_buff[*s] );
	}
	/* go to saved cursor position */
	for (;*s>*t;--*s) {
		vtbuild(con->tty->vtb, TOKEN_LEFT);
	}
	*s = con->cl_cnt;
	return 0;
}

int vconsole_deactivate(vconsole_t *con) {
	uint32_t *s; s = &con->tty->rx_cnt;
	uint32_t *t; t = &con->tty->rx_cur;
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
	return 0;
}

int vconsole_close(vconsole_t *con) {
	return 0;
}
