/**
 * @file
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#include <stdio.h>
#include <string.h>
#include <drivers/iodev.h>
#include <drivers/tty_ng.h>
#include <embox/cmd.h>
#include <cmd/shell.h>
#include <errno.h>

EMBOX_CMD(serial_con_manager);
h
#define SERIAL_N_CON 10
#define BUF_SIZE (80 * 30)

static char serial_buffer[SERIAL_N_CON * BUF_SIZE];
static int pos[SERIAL_N_CON];
static int sz[SERIAL_N_CON];
static int buf_pos = 0;
static int act_id = 0;

static const char clrscr[] =  "\x1b[2J\x1b[H";

static void serial_make_active(struct tty_buf *tty) {
	int cnt = sz[tty->id];
	int ps = pos[tty->id];

	const char *ch_buf = clrscr;
	while (*ch_buf != '\0') {
		iodev_putc(*ch_buf++);
	}

	if (cnt == BUF_SIZE) {
		while (cnt --) {
			iodev_putc(tty->out_buf[ps]);
			ps = (ps + 1) % BUF_SIZE;
		}
	} else {
		for (size_t i = 0; i < ps; i++) {
			iodev_putc(tty->out_buf[i]);
		}
	}
	act_id = tty->id;
}

static void serial_make_inactive(struct tty_buf *tty) {

}

static void serial_pc(struct tty_buf *tty, char ch) {
	if (act_id == tty->id) {
		iodev_putc(ch);
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

static int serial_con_manager(int argc, char *argv[]) {
	int n = 1;

	if (argc <= 1) {
		printf("Must be run with n: count of virtual consoles\n");
		return -EINVAL;
	}

	sscanf(argv[1], "%d", &n);

	if (n <= 0 || n > SERIAL_N_CON) {
		printf("Invalid parameter\n");
		return -EINVAL;
	}

	tty_ng_manager(n, tty_serial_init, shell_lookup("tish")->run);

	return 0;
}
