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

static char default_getc() {
	return 0;
}

static void default_putc(char ch) {
}

static char vtparse_getc(VTPARSER *parser) {
	TERMINAL* terminal = (TERMINAL*) parser->user_data;
	if (terminal->footprint_counter == 0) {
		return terminal->io->getc();
	} else {
		return 0;
	}
}

static void vtbuild_putc(VTBUILDER *builder, char ch) {
	TERMINAL* terminal = (TERMINAL*) builder->user_data;
	terminal->io->putc(ch);
}

void terminal_init(TERMINAL *terminal, TERMINAL_IO *io) {
	if (terminal == NULL) {
		return;
	}

	if (io != NULL && io->getc != NULL) {
		terminal->io->getc = io->getc;
	} else {
		terminal->io->getc = default_getc;
	}
	if (io != NULL && io->putc != NULL) {
		terminal->io->putc = io->putc;
	} else {
		terminal->io->putc = default_putc;
	}

	terminal->builder->user_data = terminal;
	vtbuild_init(terminal->builder, vtbuild_putc);

	terminal->parser->user_data = terminal;
	vtparse_init(terminal->parser, vtparse_callback, vtparse_getc);
	terminal->footprint_counter = 0;
}

static BOOL is_ctrl_token(TERMINAL_TOKEN_CTRL ctrl) {
	switch (ctrl) {
	case TERMINAL_TOKEN_CTRL_CURSOR_UP:
	case TERMINAL_TOKEN_CTRL_CURSOR_DOWN:
	case TERMINAL_TOKEN_CTRL_CURSOR_FORWARD:
	case TERMINAL_TOKEN_CTRL_CURSOR_BACKWARD:
		return TRUE;
	default:
		return FALSE;
	}
}

static BOOL transmit_ctrl_token(TERMINAL *terminal, TERMINAL_TOKEN_CTRL ctrl,
		TERMINAL_PARAMS *params) {
	if (!is_ctrl_token(ctrl)) {
		return FALSE;
	}

	vtbuild(terminal->builder, UNCTRL_ACTION(ctrl), params, UNCTRL_CODE(ctrl));
	return TRUE;
}

static BOOL transmit_char_token(TERMINAL *terminal, TERMINAL_TOKEN_CHAR ch) {
	vtbuild(terminal->builder, VT_ACTION_PRINT, NULL, ch);
	return TRUE;
}

static void vtparse_callback(VTPARSER *parser, VT_ACTION action, char ch) {
	TERMINAL *terminal = (TERMINAL *) parser->user_data;
	if (terminal->footprint_counter < VTPARSER_FOOTPRINTS_AMOUNT) {
		VTPARSER_FOOTPRINT *footprint =
				&terminal->vtparser_footprint[terminal->footprint_counter++];
		footprint->action = action;
		footprint->ch = ch;
		*footprint->params = *parser->params;
	} else {
		// TODO
		assert(FALSE);
	}

}

BOOL terminal_receive(TERMINAL *terminal, TERMINAL_TOKEN_TYPE *type,
		TERMINAL_TOKEN *token, TERMINAL_PARAMS *params) {
	int i;
	VTPARSER_FOOTPRINT *footprint;
	if (terminal->footprint_counter == 0) {
		// we don't return from vtparse until
		// callback has not been invoked at least one
		vtparse(terminal->parser);
	}
	if (terminal->footprint_counter == 0) {
		// something wrong with vtparser
		// (callback must had been invoked at least once)
		// TODO in fact this code must not be reached;
		assert(FALSE);
		return FALSE;
	}

	while (terminal->footprint_counter != 0) {
		for (i = 0; i < terminal->footprint_counter; ++i) {
			footprint = &terminal->vtparser_footprint[i];
			*type
					= (footprint->action == VT_ACTION_PRINT) ? TERMINAL_TOKEN_TYPE_CHAR
							: TERMINAL_TOKEN_TYPE_CTRL;
			switch (*type) {
			case TERMINAL_TOKEN_TYPE_CHAR:
				token->ch = footprint->ch;
				return TRUE;
			case TERMINAL_TOKEN_TYPE_CTRL:
				token->ctrl = CTRL(footprint->action, footprint->ch);
				// TODO optimization: m.b. pass pointer to params?
				if (!is_ctrl_token(token->ctrl)) {
					// unknown footprint action/code combination
					continue; // for loop
				}
				*params = *footprint->params;
				return TRUE;
			}
		}
		terminal->footprint_counter = 0;
		vtparse(terminal->parser);
	}

	// TODO in fact this code must not be reached;
	assert(FALSE);
	return FALSE;
}

BOOL terminal_transmit(TERMINAL *terminal, TERMINAL_TOKEN_TYPE type,
		TERMINAL_TOKEN *token, TERMINAL_PARAMS *params) {
	switch (type) {
	case TERMINAL_TOKEN_TYPE_CHAR:
		return transmit_char_token(terminal, token->ch);
	case TERMINAL_TOKEN_TYPE_CTRL:
		return transmit_ctrl_token(terminal, token->ctrl, params);
	default:
		return FALSE;
	}
}

