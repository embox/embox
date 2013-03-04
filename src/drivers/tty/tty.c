/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#include <stdint.h>
#include <ctype.h>
#include <util/array.h>
#include <drivers/tty.h>

void tty_init(struct tty *t, uint32_t width, uint32_t height,
		const struct tty_ops *ops, void *data) {
	t->cur_x = 0;
	t->cur_y = 0;
	t->width = width;
	t->height = height;
	t->ops = ops;
	t->data = data;

	if (!ops) {
		return;
	}

	t->ops->init(t);
	tty_clear(t);
}

void tty_scroll(struct tty *t, int32_t delta) {
	if (!t || !t->ops || !t->ops->move || !t->ops->clear) {
		return;
	}

	if (delta > 0) {
		t->ops->move(t, 0, delta, t->width, t->cur_y - delta, 0, 0);
		t->ops->clear(t, 0, t->cur_y - delta, t->width, delta);
	}
	else {
		t->ops->move(t, 0, 0, t->width, t->cur_y + delta, 0, -delta);
		t->ops->clear(t, 0, 0, t->width, -delta);
	}
	t->cur_y -= delta;
	t->back_cy -= delta;
}

static void tty_clearxy(struct tty *t, int x, int y, int tx, int ty) {
	if (!t || !t->ops || !t->ops->clear) {
		return;
	}

	t->ops->clear(t, x, y, tx, ty);
}

void tty_clear(struct tty *t) {
	tty_clearxy(t, 0, 0, t->width, t->height);
}

void tty_cursor(struct tty *t) {
	if (!t || !t->ops || !t->ops->cursor) {
		return;
	}
	t->ops->cursor(t, t->cur_x, t->cur_y);
}

static void tty_esc_putc(struct tty *t, char ch) {
	if (ch == '[') {
		t->esc_args[0] = t->esc_args[1] = 0;
		t->esc_args_count = 0;
	}
	else if (ch == ';') {
		t->esc_args[++t->esc_args_count] = 0;
	} else if (isdigit(ch) && (t->esc_args_count < ARRAY_SIZE(t->esc_args))) {
		t->esc_args[t->esc_args_count] = t->esc_args[t->esc_args_count] * 10 + ch - '0';
	}
	else {
		switch (ch) {
		case 'f': /* Move cursor + blink cursor to location v,h */
			t->cur_x = t->esc_args[1];
			if (t->cur_x != 0) {
				--t->cur_y;
			}
			t->cur_y = t->esc_args[0];
			if (t->cur_y) {
				--t->cur_x;
			}
			tty_cursor(t);
			break;
		case 'H': /* Move cursor to screen location v,h */
			t->cur_x = t->esc_args[1];
			if (t->cur_x) {
				--t->cur_x;
			}
			t->cur_y = t->esc_args[0];
			if (t->cur_y) {
				--t->cur_y;
			}
			if (t->cur_x >= t->width) {
				t->cur_x = t->width - 1;
			}
			if (t->cur_y >= t->height) {
				t->cur_y = t->height - 1;
			}
			break;
		case 'm': /* color */
			break; /* TODO */
		case 'X': /* clear n characters */
			tty_clearxy(t, t->cur_x, t->cur_y, t->esc_args[0], 1);
			break;
		case 'K': /* Clear line from cursor right */
			switch (t->esc_args[0]) {
			default:
			case 0:
				tty_clearxy(t, t->cur_x, t->cur_y, t->width - t->cur_x, 1);
				break;
			case 1:
				tty_clearxy(t, 0, t->cur_y, t->cur_x, 1);
				break;
			case 2:
				tty_clearxy(t, 0, t->cur_y, t->width, 1);
				break;
			}
			break;
		case 'J': /* Clear screen from cursor */
			switch (t->esc_args[0]) {
			default:
			case 0:
				tty_clearxy(t, 0, t->cur_y, t->width, t->height - t->cur_y);
				break;
			case 1:
				tty_clearxy(t, 0, 0, t->width, t->cur_y);
				break;
			case 2:
				tty_clearxy(t, 0, 0, t->width, t->height);
				break;
			}
			break;
		case 'D':
			t->cur_x -= t->esc_args[0];
			break;
		}
		t->esc_state = 0;
	}
}

void tty_putc(struct tty *t, char ch) {
	if (!t || !t->ops || !t->ops->putc) {
		return;
	}

	tty_cursor(t); /* erase cursor */

	if (t->esc_state) {
		tty_esc_putc(t, ch);
	}
	else {
		switch (ch) {
		case '\n':
		case '\f':
			t->cur_x = 0;
			++t->cur_y;
			if (t->cur_y >= t->height) {
				tty_scroll(t, t->cur_y - t->height + 1);
			}
			break;
		case '\r':
			t->cur_x = 0;
			break;
		case '\t':
			t->cur_x += 4;
			if (t->cur_x >= t->width) {
				++t->cur_y;
				if (t->cur_y >= t->height) {
					tty_scroll(t, t->cur_y - t->height + 1);
				}
				t->cur_x -= t->width;
			}
			break;
		case 27: /* ESC */
			t->esc_state = 1;
			break;
		case 6: /* cursor */
			++t->cur_y;
			t->cur_x += 2;
			if (t->cur_y >= t->height) {
				t->cur_y = t->height - 1;
			}
			if (t->cur_x >= t->width) {
				t->cur_x = t->width - 1;
			}
			break;
		case 1: /* home */
			t->cur_x = 0;
			break;
		case 5: /* clear to end of line */
			tty_clearxy(t, t->cur_x, t->cur_y, t->width - t->cur_x - 1, 1);
			break;
		case 8: /* back space */
			if (t->cur_x != 0)
				--t->cur_x;
			break;
		default:
			if ((unsigned char)ch >= 32) {
				if (t->cur_x >= t->width) {
					t->cur_x = 0;
					++t->cur_y;
				}
				if (t->cur_y >= t->height) {
					tty_scroll(t, t->cur_y - t->height + 1);
				}
				t->ops->putc(t, ch == '\265' ? '\346' : ch, t->cur_x, t->cur_y);
				++t->cur_x;
			}
			break;
		}
	}

	tty_cursor(t); /* draw cursor */
	t->back_cx = t->cur_x;
	t->back_cy = t->cur_y;
}
