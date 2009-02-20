/*
 * tty.c
 *
 *  Created on: 02.02.2009
 *      Author: Eldar Abusalimov
 */

#include "terminal.h"

#include "vt.h"
#include "vtparse.h"
#include "vtbuild.h"

#include "types.h"
#include "cmdline.h"
#include "uart.h"
#include "conio.h"
#include "common.h"
#include "string.h"

static void vtparse_callback(VTPARSER *parser, VT_ACTION action, char ch);

static char dummy_getc() {
	return 0;
}

static void dummy_putc(char ch) {
}

static void dummy_on_char(char ch) {
}

static void dummy_on_command(TERMINAL_COMMAND command) {
}

static char vtparse_getc(VTPARSER *parser) {
	TERMINAL* terminal = (TERMINAL*) parser->user_data;
	if (terminal->running) {
		return terminal->io->getc();
	} else {
		return 0;
	}
}

static void vtbuild_putc(VTBUILDER *builder, char ch) {
	TERMINAL* terminal = (TERMINAL*) builder->user_data;
	if (terminal->running) {
		terminal->io->putc(ch);
	}
}

void terminal_init(TERMINAL *terminal, TERMINAL_CALLBACK *cb, TERMINAL_IO *io) {
	if (terminal == NULL) {
		return;
	}

	if (io != NULL && io->getc != NULL) {
		terminal->io->getc = io->getc;
	} else {
		terminal->io->getc = dummy_getc;
	}
	if (io != NULL && io->putc != NULL) {
		terminal->io->putc = io->putc;
	} else {
		terminal->io->putc = dummy_putc;
	}

	if (cb != NULL && cb->on_char != NULL) {
		terminal->callback->on_char = cb->on_char;
	} else {
		terminal->callback->on_char = dummy_on_char;
	}
	if (cb != NULL && cb->on_command != NULL) {
		terminal->callback->on_command = cb->on_command;
	} else {
		terminal->callback->on_command = dummy_on_command;
	}

	terminal->parser->user_data = terminal;
	vtparse_init(terminal->parser, vtparse_callback, vtparse_getc);

	terminal->builder->user_data = terminal;
	vtbuild_init(terminal->builder, vtbuild_putc);
}

void terminal_start(TERMINAL *terminal) {
	terminal->running = TRUE;
	vtparse(terminal->parser);
}

void terminal_stop(TERMINAL *terminal) {
	terminal->running = FALSE;
}

void terminal_send_char(TERMINAL *terminal, char ch) {
	if (terminal->running) {
		vtbuild(terminal->builder, VT_ACTION_PRINT, NULL, ch);
	}
}

void terminal_send_command(TERMINAL *terminal, TERMINAL_COMMAND command) {
	if (terminal->running) {
		switch (command) {
		case TERMINAL_COMMAND_CURSOR_UP:
			vtbuild(terminal->builder, VT_ACTION_CSI_DISPATCH, NULL,
					ANSI_CODE_CUU);
			break;
		case TERMINAL_COMMAND_CURSOR_DOWN:
			vtbuild(terminal->builder, VT_ACTION_CSI_DISPATCH, NULL,
					ANSI_CODE_CUD);
			break;
		case TERMINAL_COMMAND_CURSOR_FORWARD:
			vtbuild(terminal->builder, VT_ACTION_CSI_DISPATCH, NULL,
					ANSI_CODE_CUF);
			break;
		case TERMINAL_COMMAND_CURSOR_BACKWARD:
			vtbuild(terminal->builder, VT_ACTION_CSI_DISPATCH, NULL,
					ANSI_CODE_CUB);
			break;
		default:
			break;
		}
	}
}

static void vtparse_callback(VTPARSER *parser, VT_ACTION action, char ch) {
	int i;
	TERMINAL *terminal = (TERMINAL *) parser->user_data;
	TERMINAL_CALLBACK * terminal_callback = terminal->callback;

	switch (action) {
	case VT_ACTION_CSI_DISPATCH:
		switch (ch) {
		case ANSI_CODE_CUU:
			terminal_callback->on_command(TERMINAL_COMMAND_CURSOR_UP);
			break;
		case ANSI_CODE_CUD:
			terminal_callback->on_command(TERMINAL_COMMAND_CURSOR_DOWN);
			break;
		case ANSI_CODE_CUF:
			terminal_callback->on_command(TERMINAL_COMMAND_CURSOR_FORWARD);
			break;
		case ANSI_CODE_CUB:
			terminal_callback->on_command(TERMINAL_COMMAND_CURSOR_BACKWARD);
			break;
		default:
			if (parser->params->length > 0) {
				int attr = parser->params->data[0];

				// TODO handle some private sequences

			}
			break;
		}

		break;
	case VT_ACTION_PRINT:
		switch (ch) {
		case '\r':
		case '\n':
			terminal_callback->on_command(TERMINAL_COMMAND_ENTER);
			break;
		case '\b':
			terminal_callback->on_command(TERMINAL_COMMAND_BACKSPACE);
			break;
		default:
			// TODO check for char range
			//			if (0x20 <= ch && ch <= 0x7E) {
			terminal_callback->on_char(ch);
			//			}
			break;
		}

		break;
	default:
		break;
	}

}
