/**
 * @file
 * @brief
 *
 * @date 04.03.13
 * @author Vita Loginova
 */

#include <drivers/vtesc.h>
#include <drivers/vtparse.h>
#include <assert.h>
#include <stddef.h>

static void vtparse_callback(struct vtparse *parser, struct vt_token *token) {
	struct vtesc_executor *executor =
			(struct vtesc_executor *) parser->user_data;
	char ch = parser->token.ch;
	short *params = parser->token.params;
	executor->token.ch = ch;

	switch (parser->token.action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
		executor->token.type = VTESC_CHARACTER;
		break;
	case VT_ACTION_CSI_DISPATCH:
	case VT_ACTION_ESC_DISPATCH:
		switch (ch) {
		case 'f': //Move cursor + blink cursor to location v,h
		case 'H': //Move cursor to screen location v,h
			executor->token.type = VTESC_CURSOR_POSITION;
			executor->token.params.cursor_position.row = params[0];
			executor->token.params.cursor_position.column = params[1];
			break;
		case 'm': //color
			executor->token.type = VTESC_SET_SGR;
			break; //TODO
		case 'X': //clear n characters
			executor->token.type = VTESC_ERASE_CHAR;
			executor->token.params.erase.n = params[0];
			break;
		case 'K': //Clear line from cursor right
			executor->token.type = VTESC_ERASE_LINE;
			executor->token.params.erase.n =
					token->params_len == 1 ? params[0] : 0;
			break;
		case 'J': //Clear screen from cursor
			executor->token.type = VTESC_ERASE_DATA;
			executor->token.params.erase.n =
					token->params_len == 1 ? params[0] : 0;
			break;
		case 'A':
			executor->token.type = VTESC_MOVE_CURSOR;
			executor->token.params.move_cursor.x = 0;
			executor->token.params.move_cursor.y =
					token->params_len == 1 ? -params[0] : -1;
			break;
		case 'B':
			executor->token.type = VTESC_MOVE_CURSOR;
			executor->token.params.move_cursor.x = 0;
			executor->token.params.move_cursor.y =
					token->params_len == 1 ? params[0] : 1;
			break;
		case 'C':
			executor->token.type = VTESC_MOVE_CURSOR;
			executor->token.params.move_cursor.x =
					token->params_len == 1 ? params[0] : 1;
			executor->token.params.move_cursor.y = 0;
			break;
		case 'D':
			executor->token.type = VTESC_MOVE_CURSOR;
			executor->token.params.move_cursor.x =
					token->params_len == 1 ? -params[0] : -1;
			executor->token.params.move_cursor.y = 0;
			break;
		case 'G'://move cursor to x position
			executor->token.type = VTESC_CURSOR_COLOMN;
			executor->token.params.cursor_position.column = params[0];
			break;
		}
		break;
	default:
		assert(0);
		break;
	}
}

struct vtesc_token *vtesc_consume(struct vtesc_executor *executor, char ch) {
	executor->token.type = VTESC_NOT_COMPLETE;
	vtparse(&executor->parser, ch);

	if (executor->token.type != VTESC_NOT_COMPLETE) {
		return &executor->token;
	}
	return NULL;
}

void vtesc_init(struct vtesc_executor *executor) {
	assert(vtparse_init(&executor->parser, NULL) != NULL);
	executor->parser.cb = vtparse_callback;
	executor->parser.user_data = executor;
}
