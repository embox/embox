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

static void vterm_scroll_up(struct vterm *vt, unsigned short delta) {
	assert(
			vt && vt->video && vt->video->ops && vt->video->ops->copy_rows
					&& vt->video->ops->clear_rows);

	vt->video->ops->copy_rows(vt->video, 0, delta, vt->video->height - delta);
	vt->video->ops->clear_rows(vt->video, vt->video->height - delta, delta);

	vt->cur_y -= delta;
}

static void vterm_clear(struct vterm *vt) {
	assert(vt && vt->video && vt->video->ops && vt->video->ops->clear_rows);

	vt->video->ops->clear_rows(vt->video, 0, vt->video->height);
}

static void vterm_cursor(struct vterm *vt) {
	assert(vt && vt->video && vt->video->ops && vt->video->ops->cursor);

	vt->video->ops->cursor(vt->video, vt->cur_x, vt->cur_y);
}

static int setup_cursor(struct vterm *vt, int x, int y) {
	vt->cur_x = x;
	if (vt->cur_x) {
		--vt->cur_x;
	}
	vt->cur_y = y;
	if (vt->cur_y) {
		--vt->cur_y;
	}
	if (vt->cur_x >= vt->video->width) {
		vt->cur_x = vt->video->width - 1;
	}
	if (vt->cur_y >= vt->video->height) {
		vt->cur_y = vt->video->height - 1;
	}
	return 0;
}

static inline void inc_line(struct vterm *vt) {
	++vt->cur_y;
	if (vt->cur_y >= vt->video->height)
		vterm_scroll_up(vt, vt->cur_y - vt->video->height + 1);
}

static void execute_printable(struct vterm *vt, char ch) {
	switch (ch) {
	case '\n':
	case '\f': /* Form feed/clear screen*/
		vt->cur_x = 0;
		inc_line(vt);
		break;
	case '\r':
		vt->cur_x = 0;
		break;
	case '\t':
		vt->cur_x += VTERM_TAB_WIDTH - vt->cur_x % VTERM_TAB_WIDTH;
		if (vt->cur_x >= vt->video->width) {
			inc_line(vt);
			vt->cur_x -= vt->video->width;
		}
		break;
	case '\b': /* back space */
		if (vt->cur_x != 0) {
			--vt->cur_x;
		}
		break;
	default:
		if (vt->cur_x >= vt->video->width) {
			vt->cur_x = 0;
			inc_line(vt);
		}
		vt->video->ops->putc(vt->video, ch, vt->cur_x, vt->cur_y);
		++vt->cur_x;
		break;
	}
}

static void erase_line_part(struct vterm *vt, unsigned short column,
		unsigned short chars) {
	for (int i = column; i < column + chars; i++) {
		vt->video->ops->putc(vt->video, ' ', i, vt->cur_y);
	}
}

static void execute_token(struct vterm *vt, struct vtesc_token *token) {
	assert(vt && vt->video && vt->video->ops && vt->video->ops->clear_rows);
	assert(token);

	switch (token->type) {
	case VTESC_CHARACTER:
		execute_printable(vt, token->ch);
		break;
	case VTESC_MOVE_CURSOR:
		setup_cursor(vt, vt->cur_x + token->params.move_cursor.x + 1,
						vt->cur_y + token->params.move_cursor.y + 1);
		break;
	case VTESC_CURSOR_POSITION:
		setup_cursor(vt, token->params.cursor_position.column,
				token->params.cursor_position.row);
		break;
	case VTESC_CURSOR_COLOMN:
		setup_cursor(vt, token->params.cursor_position.column, vt->cur_y + 1);
		break;
	case VTESC_ERASE_DATA:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the screen */
			erase_line_part(vt, vt->cur_x, vt->video->width - vt->cur_x);
			vt->video->ops->clear_rows(vt->video, vt->cur_y + 1,
					vt->video->height - vt->cur_y - 1);
			break;
		case 1: /* from cursor to beginning of the screen */
			erase_line_part(vt, 0, vt->cur_x);
			vt->video->ops->clear_rows(vt->video, 0, vt->cur_y - 1);
			break;
		case 2: /* entire screen */
			vt->video->ops->clear_rows(vt->video, 0, vt->video->height);
			break;
		}
		break;
	case VTESC_ERASE_LINE:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the line */
			erase_line_part(vt, vt->cur_x, vt->video->width - vt->cur_x);
			break;
		case 1: /* from cursor to beginning of the line */
			erase_line_part(vt, 0, vt->cur_x);
			break;
		case 2: /* entire line */
			vt->video->ops->clear_rows(vt->video, vt->cur_y, 1);
			break;
		}
		break;
	case VTESC_ERASE_CHAR:
		erase_line_part(vt, vt->cur_x, token->params.erase.n);
		break;
	case VTESC_SET_SGR: //TODO
		break;
	default:
		break;
	}

	vterm_cursor(vt);
}

static const char *vterm_key_to_esc(int keycode){
	switch (keycode) {
	case KEY_INS: return "2~";
	case KEY_HOME: return "H";
	case KEY_END: return "F";
	case KEY_PGUP: return "5~";
	case KEY_PGDN: return "6~";
	case KEY_UP: return "A";
	case KEY_DOWN: return "B";
	case KEY_LEFT: return "D";
	case KEY_RGHT: return "C";

	default: return 0;
	}
}

static char vterm_key_to_char(int keycode) {
	switch (keycode) {
	case 0:
	case KEY_CAPS:
	case KEY_SHFT:
	case KEY_CTRL:
	case KEY_ALT:
		return 0; /* no ascii symbols */

	case KEY_F1 ... KEY_F12:
		return 0; /* no ascii symbols */

	default:
		return (char) keycode;
	}
}

static int vterm_input(struct vterm *vt, struct input_event *event) {
	const char *esc_body;
	int keycode;

	if (!key_is_pressed(event)) {
		return 0;
	}

	keycode = keymap_kbd(event);
	if (keycode < 0) {
		return 0;
	}

	esc_body = vterm_key_to_esc(keycode);

	if (esc_body) {
		tty_rx_putc(&vt->tty, 0x1B, 0);
		tty_rx_putc(&vt->tty, '[', 0);

		for (const char *pch = esc_body; *pch; ++pch)
			tty_rx_putc(&vt->tty, *pch, 0);

	} else {
		char ch = vterm_key_to_char(keycode);

		if (ch)
			tty_rx_putc(&vt->tty, ch, 0);
	}

	return 0;
}

static int vterm_indev_eventhnd(struct input_dev *indev) {
	struct input_event event;

	while (0 == input_dev_event(indev, &event)) {
		vterm_input(indev->data, &event);
	}
	return 0;
}

void vterm_open_indev(struct vterm *vt, const char *name) {
	struct input_dev *dev;

	dev = input_dev_lookup(name);
	if (dev) {
		dev->data = vt;
		vt->indev = dev;
		input_dev_open(dev, &vterm_indev_eventhnd);
	}
}

void vterm_putc(struct vterm *vt, char ch) {
	struct vtesc_token *token = vtesc_consume(&vt->executor, ch);
	if (token)
		execute_token(vt, token);
}

static void vterm_tty_out_wake(struct tty *t) {
	struct vterm *vt = member_cast_out(t, struct vterm, tty);
	int ich;

	while ((ich = tty_out_getc(t)) != -1)
		vterm_putc(vt, (char) ich);
}

const struct tty_ops vterm_tty_ops = {
	.out_wake = &vterm_tty_out_wake,
};

void vterm_init(struct vterm *vt, struct vterm_video *video,
		struct input_dev *indev) {
	assert(video && video->ops && video->ops->init);

	vt->cur_x = 0;
	vt->cur_y = 0;

	vtesc_init(&vt->executor);

	vt->video = video;
	vt->indev = indev;

	vt->video->ops->init(vt->video);

	tty_init(&vt->tty, &vterm_tty_ops);

	vterm_clear(vt);
}
