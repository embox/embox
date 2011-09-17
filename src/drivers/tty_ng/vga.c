/**
 * @file
 *
 * @date 12.09.11
 * @author Anton Kozlov
 */

#include <string.h>
#include <drivers/vga.h>
#include <drivers/tty_ng.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(vga_con_manager);

#define VGA_N_CON 3
#define BUF_SIZE 4096
static vga_console_t cons[VGA_N_CON];
static char video_buffer[VGA_N_CON * BUF_SIZE];
static int buf_pos = 0;
static int act_id = 0;

static void vga_make_active(struct tty_buf *tty) {
	vga_console_t *con = (vga_console_t *) tty->out_buf;
	memcpy((char *) VIDEO, (void *) con->video, BUF_SIZE);
	blink_cursor(con->x, con->y);
	act_id = tty->id;
}

static void vga_make_inactive(struct tty_buf *tty) {

}

//TODO slightly dirty
static void vga_pc(struct tty_buf *tty, char ch) {
//	vga_console_t *con = (vga_console_t *) tty->out_buf;
	vga_putc((vga_console_t *) tty->out_buf, ch);
	if (tty->id == act_id) {
		vga_make_active(tty);
	}
#if 0
	if (tty->id == act_id) {
		char *video = (char *) VIDEO;
		video[(con->x + (con->y * con->width)) * 2 ] = ch;
		video[(con->x + (con->y * con->width)) * 2 + 1] = 0x07;
	}

	vga_putc((vga_console_t *) tty->out_buf, ch);

	if (tty->id == act_id) {
		blink_cursor(con->x, con->y);
	}
#endif
}

static void run(void) {
	char ch;
	while (1) {
		read(0, &ch, 1);
		printf("tty!%c\n", ch);
	}
}

static void tty_vga_init(struct tty_buf *tty) {
	tty->id = buf_pos;
	vga_console_init(&cons[buf_pos], 80, 25);
	cons[buf_pos].video = (vchar_t *) video_buffer + (buf_pos * BUF_SIZE);

	tty->out_buf = (char *) &cons[buf_pos];
	tty->putc = vga_pc;
	tty->make_active = vga_make_active;
	tty->make_inactive = vga_make_inactive;
	buf_pos ++;
}

static int vga_con_manager(void) {

	tty_ng_manager(VGA_N_CON, tty_vga_init, run);
	return 0;
}

