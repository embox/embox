/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <util/array.h>
#include <drivers/tty.h>
#include <drivers/vt.h>
#include <drivers/vtparse.h>
#include <assert.h>

static void tty_scroll(struct tty *t, int32_t delta) {
	if (!t || !t->ops || !t->ops->move || !t->ops->clear) {
		return;
	}

	if (delta > 0) {
		t->ops->move(t, 0, delta, t->width, t->cur_y - delta, 0, 0);
		t->ops->clear(t, 0, t->cur_y - delta, t->width, delta);
	} else {
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
	t->ops->cursor(t, t->back_cx, t->back_cy);
	t->back_cx = t->cur_x;
	t->back_cy = t->cur_y;
}

static int setup_cursor(struct tty *t, int x, int y) {
	t->cur_x = t->parser.token.params[1];
	if (t->cur_x) {
		--t->cur_x;
	}
	t->cur_y = t->parser.token.params[0];
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

static inline void inc_line(struct tty *t) {
	++t->cur_y;
	if (t->cur_y >= t->height) {
		tty_scroll(t, t->cur_y - t->height + 1);
	}
}

static void vtparse_callback(struct vtparse *parser, struct vt_token *token) {
	struct tty *t = (struct tty *) parser->user_data;
	switch (parser->token.action) {
		case VT_ACTION_PRINT:
		case VT_ACTION_EXECUTE:
			switch (parser->token.ch) {
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

					t->ops->putc(t, parser->token.ch, t->cur_x, t->cur_y);
					++t->cur_x;

					break;
				}
			break;
		case VT_ACTION_CSI_DISPATCH:
		case VT_ACTION_ESC_DISPATCH:
			switch (parser->token.ch) {
					case 'f':  //Move cursor + blink cursor to location v,h
					case 'H':  //Move cursor to screen location v,h
						setup_cursor(t, parser->token.params[1], parser->token.params[0]);
						break;
					case 'm':  //color
						break;  //TODO
					case 'X':  //clear n characters
						t->ops->clear(t, t->cur_x, t->cur_y, parser->token.params[0], 1);
						break;
					case 'K':  //Clear line from cursor right
						switch (parser->token.params[0]) {
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
					case 'J':  //Clear screen from cursor
						switch (parser->token.params[0]) {
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
						t->cur_x -= parser->token.params[0];
						break;
					}
			break;
			default:
				assert(0);
				break;
	}
}

void tty_putc(struct tty *t, char ch) {
	if (!t || !t->ops || !t->ops->putc) {
		return;
	}

	vtparse(&t->parser, ch);
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

	if (!ops || !ops->init) {
		return;
	}

	t->ops->init(t);

	assert(vtparse_init(&t->parser, NULL) != NULL);
	t->parser.cb = vtparse_callback;
	t->parser.user_data = t;

	tty_clear(t);
}
