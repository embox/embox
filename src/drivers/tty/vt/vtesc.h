/**
 * @file
 * @brief
 *
 * @date 04.03.13
 * @author Vita Loginova
 */

#ifndef VTESC_H_
#define VTESC_H_

#include <drivers/vtparse.h>

enum vtesc_token_type {
	VTESC_NOT_COMPLETE,
	VTESC_CHARACTER,
	VTESC_MOVE_CURSOR,
	VTESC_CURSOR_POSITION,
	VTESC_CURSOR_COLOMN,
	VTESC_ERASE_DATA,
	VTESC_ERASE_LINE,
	VTESC_ERASE_CHAR,
	VTESC_SET_SGR
};

struct vtesc_token {
	enum vtesc_token_type type;
	char ch;

	union {
		struct {
			short x, y;
		} move_cursor;
		struct {
			short row, column;
		} cursor_position;
		struct {
			short n;
		} erase;
	} params;
};

struct vtesc_executor {
	struct vtesc_token token;
	struct vtparse parser;
};

extern struct vtesc_token *vtesc_consume(struct vtesc_executor *executor, char ch);
extern void vtesc_init(struct vtesc_executor *executor);

#endif /* VTESC_H_ */
