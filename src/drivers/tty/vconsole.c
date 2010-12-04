/**
 * @file
 *
 * @brief Implements utilities functions for virtual console working.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 */
#include <embox/unit.h>
#include <drivers/vconsole.h>
#include <fs/file.h>

static vconsole_t def_console;
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
	*t = con->cl_cur;
	for (*s=0; *s<con->cl_cnt; ++*s) {
		uart_putc( con->tty->rx_buff[*s] = con->cl_buff[*s] );
	}
	for (;*s>*t;--*s) {
		#if 1 /* def TTY_USE_CONTROL_H_CHAR */
		uart_putc(8);
		#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
		#else
		/* vtbuild(tty_vtbuild, TOKEN_LEFT); */
		#endif
	}
	*s = con->cl_cnt;
	return 0;
}

int vconsole_deactivate(vconsole_t *con) {
	uint32_t *s; s = &con->tty->rx_cnt;
	uint32_t *t; t = &con->tty->rx_cur;
	con->cl_cnt = *s;
	con->cl_cur = *t;
	for (;*t>0;--*t) {
		uart_putc(8);
		#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
	}
	for (;*t<*s;++*t) {
		uart_putc(' ');
		#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
	}
	con->cl_buff[0] = con->tty->rx_buff[0];
	for (;*s>0;--*s) {
		con->cl_buff[*s] = con->tty->rx_buff[*s];
		#if 1 /* def TTY_USE_CONTROL_H_CHAR */
		uart_putc(8);
		#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch)
		#else
		/* vtbuild(tty_vtbuild, TOKEN_LEFT); */
		#endif
	}
	*s = *t = 0;
	return 0;
}

int vconsole_close(vconsole_t *con) {
	return 0;
}
