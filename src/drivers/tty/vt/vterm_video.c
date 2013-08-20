/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */

#include <assert.h>

#include <drivers/vterm_video.h>

#define VTERM_TAB_WIDTH 8

static void vterm_video_scroll_up(struct vterm_video *vi, unsigned short delta) {
	assert(
			vi && vi->ops && vi->ops->copy_rows
					&& vi->ops->clear_rows);

	vi->ops->copy_rows(vi, 0, delta, vi->height - delta);
	vi->ops->clear_rows(vi, vi->height - delta, delta);

	vi->cur_y -= delta;
}

static void vterm_video_clear(struct vterm_video *vi) {
	assert(vi && vi && vi->ops && vi->ops->clear_rows);

	vi->ops->clear_rows(vi, 0, vi->height);
}

static void vterm_video_cursor(struct vterm_video *vi) {
	assert(vi && vi && vi->ops && vi->ops->cursor);

	vi->ops->cursor(vi, vi->cur_x, vi->cur_y);
}

static int setup_cursor(struct vterm_video *vi, int x, int y) {
	vi->cur_x = x;
	if (vi->cur_x) {
		--vi->cur_x;
	}
	vi->cur_y = y;
	if (vi->cur_y) {
		--vi->cur_y;
	}
	if (vi->cur_x >= vi->width) {
		vi->cur_x = vi->width - 1;
	}
	if (vi->cur_y >= vi->height) {
		vi->cur_y = vi->height - 1;
	}
	return 0;
}

static inline void inc_line(struct vterm_video *vi) {
	++vi->cur_y;
	if (vi->cur_y >= vi->height)
		vterm_video_scroll_up(vi, vi->cur_y - vi->height + 1);
}

static void execute_printable(struct vterm_video *vi, char ch) {
	switch (ch) {
	case '\a': /* bell */
		break;
	case '\n':
	case '\f': /* Form feed/clear screen*/
		vi->cur_x = 0;
		inc_line(vi);
		break;
	case '\r':
		vi->cur_x = 0;
		break;
	case '\t':
		vi->cur_x += VTERM_TAB_WIDTH - vi->cur_x % VTERM_TAB_WIDTH;
		if (vi->cur_x >= vi->width) {
			inc_line(vi);
			vi->cur_x -= vi->width;
		}
		break;
	case '\b': /* back space */
		if (vi->cur_x != 0) {
			--vi->cur_x;
		}
		break;
	default:
		if (vi->cur_x >= vi->width) {
			vi->cur_x = 0;
			inc_line(vi);
		}
		vi->ops->putc(vi, ch, vi->cur_x, vi->cur_y);
		++vi->cur_x;
		break;
	}
}

static void erase_line_part(struct vterm_video *vi, unsigned short column,
		unsigned short chars) {
	for (int i = column; i < column + chars; i++) {
		vi->ops->putc(vi, ' ', i, vi->cur_y);
	}
}

static void execute_token(struct vterm_video *vi, struct vtesc_token *token) {
	assert(vi && vi && vi->ops && vi->ops->clear_rows);
	assert(token);

	switch (token->type) {
	case VTESC_CHARACTER:
		execute_printable(vi, token->ch);
		break;
	case VTESC_MOVE_CURSOR:
		setup_cursor(vi, vi->cur_x + token->params.move_cursor.x + 1,
						vi->cur_y + token->params.move_cursor.y + 1);
		break;
	case VTESC_CURSOR_POSITION:
		setup_cursor(vi, token->params.cursor_position.column,
				token->params.cursor_position.row);
		break;
	case VTESC_CURSOR_COLOMN:
		setup_cursor(vi, token->params.cursor_position.column, vi->cur_y + 1);
		break;
	case VTESC_ERASE_DATA:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the screen */
			erase_line_part(vi, vi->cur_x, vi->width - vi->cur_x);
			vi->ops->clear_rows(vi, vi->cur_y + 1,
					vi->height - vi->cur_y - 1);
			break;
		case 1: /* from cursor to beginning of the screen */
			erase_line_part(vi, 0, vi->cur_x);
			vi->ops->clear_rows(vi, 0, vi->cur_y - 1);
			break;
		case 2: /* entire screen */
			vi->ops->clear_rows(vi, 0, vi->height);
			break;
		}
		break;
	case VTESC_ERASE_LINE:
		switch (token->params.erase.n) {
		default:
		case 0: /* from cursor to end of the line */
			erase_line_part(vi, vi->cur_x, vi->width - vi->cur_x);
			break;
		case 1: /* from cursor to beginning of the line */
			erase_line_part(vi, 0, vi->cur_x);
			break;
		case 2: /* entire line */
			vi->ops->clear_rows(vi, vi->cur_y, 1);
			break;
		}
		break;
	case VTESC_ERASE_CHAR:
		erase_line_part(vi, vi->cur_x, token->params.erase.n);
		break;
	case VTESC_SET_SGR: //TODO
		break;
	default:
		break;
	}

	vterm_video_cursor(vi);
}

void vterm_video_putc(struct vterm_video *vi, char ch) {
	struct vtesc_token *token = vtesc_consume(&vi->executor, ch);
	if (token)
		execute_token(vi, token);
}

int vterm_video_init(struct vterm_video *vi, const struct vterm_video_ops *ops,
		unsigned short width, unsigned short height) {

	vi->ops = ops;
	vi->width = width;
	vi->height = height;

	vi->cur_x = 0;
	vi->cur_y = 0;

	vtesc_init(&vi->executor);

	vterm_video_clear(vi);
	return 0;
}

