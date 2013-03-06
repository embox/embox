/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <util/array.h>
#include <drivers/video_term.h>
#include <drivers/vt.h>
#include <drivers/vtparse.h>
#include <assert.h>

static void vterm_scroll(struct vterm *t, int32_t delta) {
	if (!t || !t->ops || !t->ops->scroll) {
		return;
	}

	t->ops->scroll(t, delta);
	t->cur_y -= delta;
	t->back_cy -= delta;
}

static void vterm_clearxy(struct vterm *t, int x, int y, int tx, int ty) {
	if (!t || !t->ops || !t->ops->clear) {
		return;
	}

	t->ops->clear(t, x, y, tx, ty);
}

void vterm_clear(struct vterm *t) {
	vterm_clearxy(t, 0, 0, t->width, t->height);
}

void vterm_cursor(struct vterm *t) {
	if (!t || !t->ops || !t->ops->cursor) {
		return;
	}
	t->ops->cursor(t, t->back_cx, t->back_cy);
	t->back_cx = t->cur_x;
	t->back_cy = t->cur_y;
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
	if (t->cur_x >= t->width) {
		t->cur_x = t->width - 1;
	}
	if (t->cur_y >= t->height) {
		t->cur_y = t->height - 1;
	}
	return 0;
}

static inline void inc_line(struct vterm *t) {
	++t->cur_y;
	if (t->cur_y >= t->height) {
		vterm_scroll(t, t->cur_y - t->height + 1);
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
}

static void execute_token(struct vterm *t, struct vtesc_token *token) {
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
	case VTESC_ERASE_DATA:
		switch (token->params.erase.n) {
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
	case VTESC_ERASE_LINE:
		switch (token->params.erase.n) {
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
	case VTESC_ERASE_CHARASTER:
		t->ops->clear(t, t->cur_x, t->cur_y, token->params.erase.n, 1);
		break;
	case VTESC_SET_SGR: //TODO
		break;
	default:
		break;
	}
}

void vterm_putc(struct vterm *t, char ch) {
	struct vtesc_token *token;

	if (!t || !t->ops || !t->ops->putc) {
		return;
	}

	token = vtesc_consume(&t->executor, ch);
	if (token) {
		execute_token(t, token);
	}
	vterm_cursor(t);
}

void vterm_init(struct vterm *t, uint32_t width, uint32_t height,
		const struct vterm_ops *ops, void *data) {
	t->cur_x = 0;
	t->cur_y = 0;
	t->width = width;
	t->height = height;
	t->ops = ops;
	t->data = data;

	vtesc_init(&t->executor);

	if (!ops || !ops->init) {
		return;
	}

	t->ops->init(t);

	vterm_clear(t);
}
