/**
 * @file
 *
 * @date 28.02.2009
 * @author Eldar Abusalimov
 */
#ifndef SCREEN_H_
#define SCREEN_H_

#include <types.h>
#include "cmdline.h"
#include <drivers/terminal.h>

struct _SCREEN;

typedef struct _SCREEN_CALLBACK {
#define CALLBACK_ENTRY(key) int (*on_##key)(struct _SCREEN_CALLBACK *, \
	struct _SCREEN *view, int by);
	CALLBACK_ENTRY(char)
	CALLBACK_ENTRY(cursor_up)
	CALLBACK_ENTRY(cursor_down)
	CALLBACK_ENTRY(cursor_left)
	CALLBACK_ENTRY(cursor_right)
	CALLBACK_ENTRY(new_line)
	CALLBACK_ENTRY(backspace)
	CALLBACK_ENTRY(delete)
	CALLBACK_ENTRY(insert)
	CALLBACK_ENTRY(home)
	CALLBACK_ENTRY(end)
	CALLBACK_ENTRY(tab)
	CALLBACK_ENTRY(etx)
	CALLBACK_ENTRY(eot)
	CALLBACK_ENTRY(dc2)
	CALLBACK_ENTRY(dc4)
	CALLBACK_ENTRY(ack)
	void *outer;
} SCREEN_CALLBACK;

typedef TERMINAL_IO SCREEN_IO;

typedef struct _SCREEN {
	SCREEN_CALLBACK *callback;
	TERMINAL terminal[1];
	char string[CMDLINE_MAX_LENGTH + 1]; /**< line itself */
#if 0
	int length; /**< string length */
#endif
	int cursor;   /**< cursor current position */
	const char *prompt;
#if 0
	int prompt_len;
#endif
	bool running;
} SCREEN;

SCREEN * screen_init(SCREEN *, SCREEN_IO *io);

void screen_out_show_prompt(SCREEN *, const char *prompt);
void screen_out_puts(SCREEN *, const char *str);
void screen_out_update(SCREEN *, CMDLINE *cmdline);
void screen_in_start(SCREEN *, SCREEN_CALLBACK *cb);
void screen_in_stop(SCREEN *);

#endif /* SCREEN_H_ */
