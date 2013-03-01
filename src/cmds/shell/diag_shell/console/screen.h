/**
 * @file
 *
 * @date 28.02.09
 * @author Eldar Abusalimov
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "cmdline.h"
#include <drivers/terminal.h>
#include <stdbool.h>


struct _CONSOLE;

typedef struct _SCREEN_CALLBACK {
	struct _CONSOLE *outer; /* CONSOLE */
} SCREEN_CALLBACK;

typedef TERMINAL_IO SCREEN_IO;


typedef struct _SCREEN {
	SCREEN_CALLBACK *callback;
	TERMINAL terminal[1];
	char string[CMDLINE_MAX_LENGTH + 1]; /**< line itself */
	int cursor;   /**< cursor current position */
	const char *prompt;
	bool running;
} SCREEN;

SCREEN * screen_init(SCREEN *, SCREEN_IO *io);

void screen_out_show_prompt(SCREEN *, const char *prompt);
void screen_out_puts(SCREEN *, const char *str);
void screen_out_update(SCREEN *, CMDLINE *cmdline);
void screen_in_start(SCREEN *, SCREEN_CALLBACK *cb);
void screen_in_stop(SCREEN *);

#endif /* SCREEN_H_ */
