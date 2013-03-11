/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol, Vita Loginova
 */

#include <stddef.h>
#include <util/array.h>
#include <drivers/video_term.h>
#include <drivers/vt.h>
#include <drivers/vtparse.h>
#include <assert.h>

static void vterm_scroll(struct vterm *t, short delta) {
	if (!t || !t->ops || !t->ops->copy_rows || !t->ops->clear_rows) {
		return;
	}

	if (delta > 0){
		t->ops->copy_rows(t, 0, delta, t->height - delta);
		t->ops->clear_rows(t, t->height - delta, delta);
	} else {
		t->ops->copy_rows(t, -delta, 0, t->height + delta);
		t->ops->clear_rows(t, 0, -delta);
	}

	t->cur_y -= delta;
	t->back_cy -= delta;
}

void vterm_clear(struct vterm *t) {
	if (!t || !t->ops || !t->ops->clear_rows) {
			return;
		}

	t->ops->clear_rows(t, 0, t->height);
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

static void erase_line_part(struct vterm *t, unsigned short column, unsigned short chars){
	for (int i = column; i < column + chars; i++){
		t->ops->putc(t, ' ', column, t->cur_y);
	}
}

static void execute_token(struct vterm *t, struct vtesc_token *token) {
	if (!t || !t->ops || !t->ops->clear_rows) {
			return;
		}

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
		case 0: /* from cursor to end of the screen */
			erase_line_part(t, t->cur_x, t->width - t->cur_x);
			t->ops->clear_rows(t, t->cur_y + 1, t->height - t->cur_y - 1);
			break;
		case 1: /* from cursor to beginning of the screen */
			erase_line_part(t, 0, t->cur_x);
			t->ops->clear_rows(t, 0, t->cur_y - 1);
			break;
		case 2: /* entire screen */
			t->ops->clear_rows(t, 0, t->height);
			break;
		}
		break;
	case VTESC_ERASE_LINE:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the line */
			erase_line_part(t, t->cur_x, t->width - t->cur_x);
			break;
		case 1: /* from cursor to beginning of the line */
			erase_line_part(t, 0, t->cur_x);
			break;
		case 2: /* entire line */
			t->ops->clear_rows(t, t->cur_y, 1);
			break;
		}
		break;
	case VTESC_ERASE_CHARACTER:
		erase_line_part(t, t->cur_x, token->params.erase.n);
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

void vterm_init(struct vterm *t, unsigned short width, unsigned short height,
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
