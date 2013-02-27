/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#include <types.h>
#include <ctype.h>
#include <util/array.h>
#include <drivers/tty.h>
#include <drivers/vt.h>



static void tty_scroll(struct tty *t, int32_t delta) {
	if (delta > 0) {
		t->ops->move(t, 0, delta, t->width, t->cur_y - delta, 0, 0);
		t->ops->clear(t, 0, t->cur_y - delta, t->width, delta);
		t->cur_y -= delta;
	} else {
		t->ops->move(t, 0, 0, t->width, t->cur_y + delta, 0, -delta);
		t->ops->clear(t, 0, 0, t->width, -delta);
		t->cur_y += -delta;
	}
}

static void tty_clear(struct tty *t) {
	t->ops->clear(t, 0, 0, t->width, t->height);
}

static void tty_cursor(struct tty *t) {
	t->ops->cursor(t, t->cur_x, t->cur_y);
}

static int setup_cursor(struct tty *t, int x, int y) {
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
	return 0;
}

static int esc_seq_parse(struct tty *t, char ch) {
	switch (ch) {
	case 'f': /* Move cursor + blink cursor to location v,h */
	case 'H': /* Move cursor to screen location v,h */
		setup_cursor(t, t->esc_args[1], t->esc_args[0]);
		break;
	case 'm': /* color */
		break; /* TODO */
	case 'X': /* clear n characters */
		t->ops->clear(t, t->cur_x, t->cur_y, t->esc_args[0], 1);
		break;
	case 'K': /* Clear line from cursor right */
		switch (t->esc_args[0]) {
		default:
		case 0:
			t->ops->clear(t, t->cur_x, t->cur_y, t->width - t->cur_x, 1);
			break;
		case 1:
			t->ops->clear(t, 0, t->cur_y, t->cur_x, 1);
			break;
		case 2:
			t->ops->clear(t, 0, t->cur_y, t->width, 1);
			break;
		}
		break;
	case 'J': /* Clear screen from cursor */
		switch (t->esc_args[0]) {
		default:
		case 0:
			t->ops->clear(t, 0, t->cur_y, t->width, t->height - t->cur_y);
			break;
		case 1:
			t->ops->clear(t, 0, 0, t->width, t->cur_y);
			break;
		case 2:
			t->ops->clear(t, 0, 0, t->width, t->height);
			break;
		}
		break;
	case 'D':
		t->cur_x -= t->esc_args[0];
		break;
	}
	t->esc_state = 0;
	return 0;
}

static int tty_esc_putc(struct tty *t, char ch) {
	if(ESC == ch) {
		t->esc_state = 1;
		return 0;
	}
	if (0 == t->esc_state) {
		return 1;
	}

	if (CSI == ch) {
		t->esc_args[0] = t->esc_args[1] = 0;
		t->esc_args_count = 0;
		return 0;
	}
	else if (ch == ';') {
		t->esc_args[++t->esc_args_count] = 0;
		return 0;
	} else if (isdigit(ch) && (t->esc_args_count < ARRAY_SIZE(t->esc_args))) {
		t->esc_args[t->esc_args_count] = t->esc_args[t->esc_args_count] * 10 + ch - '0';
		return 0;
	}
	else {
		return esc_seq_parse(t, ch);
	}
	return 0;
}

static inline void inc_line(struct tty *t) {
	++t->cur_y;
	if (t->cur_y >= t->height) {
		tty_scroll(t, t->cur_y - t->height + 1);
	}
}

void tty_putc(struct tty *t, char ch) {
	if(0 == tty_esc_putc(t, ch)) {
		tty_cursor(t);
		return;
	}
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
		t->cur_x += 4;
		if (t->cur_x >= t->width) {
			inc_line(t);
			t->cur_x -= t->width;
		}
		break;
	case '\b': /* back space */
		if (t->cur_x != 0)
			--t->cur_x;
		break;

	default:
		if (t->cur_x >= t->width) {
			t->cur_x = 0;
			inc_line(t);
		}

		t->ops->putc(t, ch, t->cur_x, t->cur_y);
		++t->cur_x;

		break;
	}

	tty_cursor(t);
}


void tty_init(struct tty *t, uint32_t width, uint32_t height,
		const struct tty_ops *ops, void *data) {
	t->cur_x = 0;
	t->cur_y = 0;
	t->width = width;
	t->height = height;
	t->ops = ops;
	t->data = data;

	t->ops->init(t);
	tty_clear(t);
}
