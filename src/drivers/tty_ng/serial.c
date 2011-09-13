/**
 * @file
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#include <string.h>
#include <drivers/serial.h>
#include <drivers/tty_ng.h>
#include <embox/unit.h>
#include <cmd/shell.h>

EMBOX_UNIT_INIT(serial_con_manager);

#define SERIAL_N_CON 3
#define BUF_SIZE (80 * 30)

static char serial_buffer[SERIAL_N_CON * BUF_SIZE];
static int pos[SERIAL_N_CON];
static int sz[SERIAL_N_CON];
static int buf_pos = 0;
static int act_id = 0;

const char clrscr[] =  "\x1b[2J\x1b[H";

static void serial_make_active(struct tty_buf *tty) {
	int cnt = sz[tty->id];
	int ps = pos[tty->id];

	const char *ch_buf = clrscr;
	while (*ch_buf != '\0') {
		uart_putc(*ch_buf++);
	}

	if (cnt == BUF_SIZE) {
		while (cnt --) {
			uart_putc(tty->out_buf[ps]);
			ps = (ps + 1) % BUF_SIZE;
		}
	} else {
		for (int i = 0; i < ps; i++) {
			uart_putc(tty->out_buf[i]);
		}
	}
	act_id = tty->id;
}

static void serial_make_inactive(struct tty_buf *tty) {

}

static void serial_pc(struct tty_buf *tty, char ch) {
	if (act_id == tty->id) {
		uart_putc(ch);
	}

	tty->out_buf[pos[tty->id]] = ch;

	if (sz[tty->id] < BUF_SIZE) {
		sz[tty->id]++;
	}

	pos[tty->id] = (pos[tty->id] + 1) % BUF_SIZE;

}

static void tty_serial_init(struct tty_buf *tty) {
	tty->id = buf_pos;
	tty->out_buf = serial_buffer + (buf_pos * BUF_SIZE);
	tty->putc = serial_pc;
	tty->make_active = serial_make_active;
	tty->make_inactive = serial_make_inactive;
	buf_pos ++;
}

static void run(void) {
	char ch;
	while (1) {
		read(0, &ch, 1);
		printf("tty!%c\n", ch);
	}
}

static int serial_con_manager(void) {

	tty_ng_manager(SERIAL_N_CON, tty_serial_init, run);
	return 0;
}
