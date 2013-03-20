/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 * @author Vita Loginova
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <util/array.h>
#include <util/member.h>
#include <drivers/video_term.h>
#include <drivers/keyboard.h>
#include <drivers/input/input_dev.h>
#include <drivers/input/keymap.h>
#include <drivers/tty.h>

#define VTERM_TAB_WIDTH 8

static void vterm_scroll_up(struct vterm *t, unsigned short delta) {
	assert(
			t && t->video && t->video->ops && t->video->ops->copy_rows
					&& t->video->ops->clear_rows);

	t->video->ops->copy_rows(t->video, 0, delta, t->video->height - delta);
	t->video->ops->clear_rows(t->video, t->video->height - delta, delta);

	t->cur_y -= delta;
}

static void vterm_clear(struct vterm *t) {
	assert(t && t->video && t->video->ops && t->video->ops->clear_rows);

	t->video->ops->clear_rows(t->video, 0, t->video->height);
}

static void vterm_cursor(struct vterm *t) {
	assert(t && t->video && t->video->ops && t->video->ops->cursor);

	t->video->ops->cursor(t->video, t->cur_x, t->cur_y);
}

static int setup_cursor(struct vterm *t, int x, int y) {
	t->cur_x = x;
	if (t->cur_x) {
		--t->cur_x;
	}
	t->cur_y = y;
	if (t->cur_y) {
		--t->cur_y;
	}
	if (t->cur_x >= t->video->width) {
		t->cur_x = t->video->width - 1;
	}
	if (t->cur_y >= t->video->height) {
		t->cur_y = t->video->height - 1;
	}
	return 0;
}

static inline void inc_line(struct vterm *t) {
	++t->cur_y;
	if (t->cur_y >= t->video->height) {
		vterm_scroll_up(t, t->cur_y - t->video->height + 1);
	}
}

static void execute_printable(struct vterm *t, char ch) {
	switch (ch) {
	case '\n':
	case '\f': /* Form feed/clear screen*/
		t->cur_x = 0;
		inc_line(t);
		break;
	case '\r':
		t->cur_x = 0;
		break;
	case '\t':
		t->cur_x += VTERM_TAB_WIDTH;
		if (t->cur_x >= t->video->width) {
			inc_line(t);
			t->cur_x -= t->video->width;
		}
		break;
	case '\b': /* back space */
		if (t->cur_x != 0) {
			--t->cur_x;
		}
		break;
	default:
		if (t->cur_x >= t->video->width) {
			t->cur_x = 0;
			inc_line(t);
		}
		t->video->ops->putc(t->video, ch, t->cur_x, t->cur_y);
		++t->cur_x;
		break;
	}
}

static void erase_line_part(struct vterm *t, unsigned short column,
		unsigned short chars) {
	for (int i = column; i < column + chars; i++) {
		t->video->ops->putc(t->video, ' ', column, t->cur_y);
	}
}

static void execute_token(struct vterm *t, struct vtesc_token *token) {
	assert(t && t->video && t->video->ops && t->video->ops->clear_rows);

	switch (token->type) {
	case VTESC_CHARACTER:
		execute_printable(t, token->ch);
		break;
	case VTESC_MOVE_CURSOR:
		t->cur_x += token->params.move_cursor.x;
		t->cur_y += token->params.move_cursor.y;
		break;
	case VTESC_CURSOR_POSITION:
		setup_cursor(t, token->params.cursor_position.row,
				token->params.cursor_position.column);
		break;
	case VTESC_CURSOR_COLOMN:
		setup_cursor(t, token->params.cursor_position.column, t->cur_y+1);
		break;
	case VTESC_ERASE_DATA:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the screen */
			erase_line_part(t, t->cur_x, t->video->width - t->cur_x);
			t->video->ops->clear_rows(t->video, t->cur_y + 1,
					t->video->height - t->cur_y - 1);
			break;
		case 1: /* from cursor to beginning of the screen */
			erase_line_part(t, 0, t->cur_x);
			t->video->ops->clear_rows(t->video, 0, t->cur_y - 1);
			break;
		case 2: /* entire screen */
			t->video->ops->clear_rows(t->video, 0, t->video->height);
			break;
		}
		break;
	case VTESC_ERASE_LINE:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the line */
			erase_line_part(t, t->cur_x, t->video->width - t->cur_x);
			break;
		case 1: /* from cursor to beginning of the line */
			erase_line_part(t, 0, t->cur_x);
			break;
		case 2: /* entire line */
			t->video->ops->clear_rows(t->video, t->cur_y, 1);
			break;
		}
		break;
	case VTESC_ERASE_CHAR:
		erase_line_part(t, t->cur_x, token->params.erase.n);
		break;
	case VTESC_SET_SGR: //TODO
		break;
	default:
		break;
	}
}

static const unsigned char esc_start[] = { 0x1B, 0x5B }; /* esc, '[' */

static int vterm_indev_eventhnd(struct input_dev *indev) {
	unsigned char ascii_buff[4];
	struct input_event event;
	int keycode;
	int seq_len = 0;

	if (0 != input_dev_event(indev, &event)) {
		return 0;
	}
	if (!key_is_pressed(&event)) {
		return 0;
	}
	keycode = keymap_kbd(&event);

	if (keycode < 0) {
		return 0;
	}

	switch (keycode) {
	case 0:
	case KEY_CAPS:
	case KEY_SHFT:
	case KEY_CTRL:
	case KEY_ALT:
		return 0; /* no ascii symbols */

	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
	case KEY_F5:
	case KEY_F6:
	case KEY_F7:
	case KEY_F8:
	case KEY_F9:
	case KEY_F10:
	case KEY_F11:
	case KEY_F12:
		return 0; /* no ascii symbols */

	case KEY_INS:
		/*0x7e325b1b */
		seq_len = 4;
		ascii_buff[2] = '2';
		ascii_buff[3] = '~';
		break;
	case KEY_HOME:
		/* 0x485b1b */
		seq_len = 3;
		ascii_buff[2] = 'H';
		break;
	case KEY_END:
		/* 0x465b1b */
		seq_len = 3;
		ascii_buff[2] = 'F';
		break;
	case KEY_PGUP:
		/* 0x7e355b1b */
		seq_len = 4;
		ascii_buff[2] = '5';;
		ascii_buff[3] = '~';
		break;
	case KEY_PGDN:
		/* 0x7e365b1b */
		seq_len = 4;
		ascii_buff[2] = '6';
		ascii_buff[3] = '~';
		break;
	case KEY_UP:
		/* 0x415b1b */
		seq_len = 3;
		ascii_buff[2] = 'A';
		break;
	case KEY_DOWN:
		/* 0x425b1b */
		seq_len = 3;
		ascii_buff[2] = 'B';
		break;
	case KEY_LEFT:
		/* 0x445b1b */
		seq_len = 3;
		ascii_buff[2] = 'D';
		break;
	case KEY_RGHT:
		/* 0x435b1b */
		seq_len = 3;
		ascii_buff[2] = 'C';
		break;

	default:
		seq_len = 1;
		ascii_buff[0] = (unsigned char) keycode;
		break;
	}
	if (seq_len > 1) {
		memcpy(ascii_buff, esc_start, sizeof(esc_start));
	}
	for (int i = 0; i < seq_len; i++) {
		//vterm_putc((struct vterm *) indev->data, ascii_buff[i]);
		tty_rx_putc(&((struct vterm *) indev->data)->tty, ascii_buff[i], 0);
	}
	return 0;
}

void vterm_open_indev(struct vterm *t, const char *name) {
	struct input_dev *dev;

	dev = input_dev_lookup(name);
	if (dev) {
		dev->data = t;
		t->indev = dev;
		input_dev_open(dev, &vterm_indev_eventhnd);
	}
}

void vterm_putc(struct vterm *t, char ch) {
	struct vtesc_token *token;

	assert(t && t->video && t->video->ops && t->video->ops->putc);

	token = vtesc_consume(&t->executor, ch);
	if (token) {
		execute_token(t, token);
	}
	vterm_cursor(t);
}

void vterm_tty_tx_char(struct tty * tty, char ch) {
	struct vtesc_token *token;
	struct vterm *vterm = member_cast_out(tty, struct vterm, tty);

	assert(vterm && vterm->video && vterm->video->ops && vterm->video->ops->putc);

	token = vtesc_consume(&vterm->executor, ch);
	if (token) {
		execute_token(vterm, token);
	}
	vterm_cursor(vterm);
}

struct tty_ops vterm_tty_ops = {
	.tx_char = &vterm_tty_tx_char
};

void vterm_init(struct vterm *t, struct vterm_video *video,
		struct input_dev *indev) {
	assert(video && video->ops && video->ops->init);

	t->cur_x = 0;
	t->cur_y = 0;

	vtesc_init(&t->executor);

	t->video = video;
	t->indev = indev;

	t->video->ops->init(t->video);

	tty_init(&t->tty, &vterm_tty_ops);

	vterm_clear(t);
}
