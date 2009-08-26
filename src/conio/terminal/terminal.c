/*
 * Terminal driver.
 *
 * Allows us to operate high-level tokens instead of simple chars.
 * It also provides some common ANSI/VT100 terminal control sequences.
 *
 * This entity is backed by VTParse and VTBuild modules.
 *
 * Author: Eldar Abusalimov
 */

#include "terminal.h"

#include "vt.h"
#include "vtparse.h"
#include "vtbuild.h"

#include "cmdline.h"
#include "uart.h"
#include "conio/conio.h"
#include "common.h"
#include "string.h"
#include "stdarg.h"

static void vtparse_callback(VTPARSER*, VT_TOKEN*);

static void vtbuild_putc(VTBUILDER *builder, char ch) {
	TERMINAL* terminal = (TERMINAL*) builder->user_data;
	terminal->io->putc(ch);
}

TERMINAL * terminal_init(TERMINAL *this, TERMINAL_IO *io) {
	if (this == NULL) {
		return NULL;
	}

	if (io != NULL && io->getc != NULL && io->putc != NULL) {
		this->io->getc = io->getc;
		this->io->putc = io->putc;
	} else {
		return NULL;
	}

	this->builder->user_data = this;
	if (vtbuild_init(this->builder, vtbuild_putc) == NULL) {
		return NULL;
	}

	this->parser->user_data = this;
	vtparse_init(this->parser, vtparse_callback);

	this->vt_token_queue_head = 0;
	this->vt_token_queue_len = 0;

	return this;
}

static BOOL is_valid_action(VT_ACTION action) {
	switch (action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
	case VT_ACTION_CS_DISPATCH:
	case VT_ACTION_ESC_DISPATCH:
		/* ok */
		return TRUE;
	case VT_ACTION_OSC_START:
	case VT_ACTION_OSC_PUT:
	case VT_ACTION_OSC_END:
		/* Operating System Command */
		// ignore them as unused in our system
		//  -- Eldar
	case VT_ACTION_HOOK:
	case VT_ACTION_PUT:
	case VT_ACTION_UNHOOK:
		/* device control strings */
		// ignore them as unused in our system
		//  -- Eldar
	case VT_ACTION_IGNORE:
	case VT_ACTION_COLLECT:
	case VT_ACTION_PARAM:
	case VT_ACTION_CLEAR:
		// ignore as VTParser internal states
	default:
		/* unknown */
		return FALSE;
	}
}

static VT_TOKEN *vt_from_term_token(TERMINAL_TOKEN terminal_token,
		TERMINAL_TOKEN_PARAMS *params) {
	static VT_TOKEN vt_token[1];

	VT_ACTION action = DECODE_ACTION(terminal_token);
	char char1 = DECODE_CHAR1(terminal_token);
	char char2 = DECODE_CHAR2(terminal_token);
	char code = DECODE_CODE(terminal_token);

	if (!is_valid_action(action)) {
		return NULL;
	}

	vt_token->action = action;

	vt_token->attrs_len = 0;
	if (char1) {
		vt_token->attrs[vt_token->attrs_len++] = char1;
	}
	if (char2) {
		vt_token->attrs[vt_token->attrs_len++] = char2;
	}

	vt_token->ch = code;
	vt_token->params = params->data;
	vt_token->params_len = params->length;

	return vt_token;
}

static TERMINAL_TOKEN term_from_vt_token(VT_TOKEN *vt_token) {
	if (vt_token == NULL) {
		return TERMINAL_TOKEN_EMPTY;
	}

	VT_ACTION action = vt_token->action;
	if (!is_valid_action(action)) {
		return TERMINAL_TOKEN_EMPTY;
	}

	char *a = vt_token->attrs;
	int a_len = vt_token->attrs_len;
	char char1 = (a_len > 0) ? a[0] : '\0';
	char char2 = (a_len > 1) ? a[1] : '\0';
	char code = vt_token->ch;

	return ENCODE(action,char1,char2, code);
}

static TERMINAL_TOKEN_PARAMS *terminal_prepare_params(TERMINAL *this,
		int length, va_list args) {
	TERMINAL_TOKEN_PARAMS *params = this->default_params;
	if (length < 0) {
		length = 0;
	}
	params->length = length;
	int i;
	for (i = 0; i < length; ++i) {
		params->data[i] = va_arg(args, INT32);
	}

	return params;
}

static void vtparse_callback(VTPARSER *parser, VT_TOKEN *token) {
	TERMINAL *terminal = (TERMINAL *) parser->user_data;
	int *queue_len = &terminal->vt_token_queue_len;
	if (*queue_len < VTPARSER_TOKEN_QUEUE_AMOUNT) {
		terminal->vt_token_queue[terminal->vt_token_queue_head + (*queue_len)++]
				= *token;
	} else {
		// TODO
		assert(FALSE);
	}

}

//static char *ACTION_NAMES[] = { "<no action>", "CLEAR", "COLLECT", "CSI_DISPATCH",
//		"ESC_DISPATCH", "EXECUTE", "HOOK", "IGNORE", "OSC_END", "OSC_PUT",
//		"OSC_START", "PARAM", "PRINT", "PUT", "UNHOOK", };

/*
 * Yes, this routine is rather ugly, but I have not found another way
 * to implement synchronous token receiving
 * without involving parser callback
 *
 * TODO describe this shit
 */
BOOL terminal_receive(TERMINAL *this, TERMINAL_TOKEN *token,
		TERMINAL_TOKEN_PARAMS *params) {
	if (this == NULL) {
		return FALSE;
	}

	int *p_len = &this->vt_token_queue_len;
	int *p_head = &this->vt_token_queue_head;
	while (*p_len == 0) {
		// we don't return from vtparse until
		// callback has not been invoked at least once.
		vtparse(this->parser, this->io->getc());
		// so vt_token_queue_len is greater then 0 at this line
		// (see vtparse_callback algorithm)
	}
	assert(*p_len != 0);

	VT_TOKEN *vt_token;
	// Almost surely this loop will be executed only once
	while (*p_len != 0) {
		for (; *p_head < *p_len; (*p_head)++) {
			vt_token = &this->vt_token_queue[*p_head];
			if ((*token = term_from_vt_token(vt_token)) != TERMINAL_TOKEN_EMPTY) {
				if (params != NULL) {
					*params->data = *vt_token->params;
					params->length = vt_token->params_len;
				}
				(*p_len)--;
				//				TRACE("%s %d(%x,%x) %x\n", ACTION_NAMES[vt_token->action],
				//						vt_token->attrs_len, vt_token->attrs[0],
				//						vt_token->attrs[1], vt_token->ch);
				return TRUE;
			}
		}
		// there were no any valid tokens in the queue,
		// so reset queue head and length indexes
		// and run parser again
		// in general case this code should not be reached
		*p_head = 0;
		*p_len = 0;
		while (*p_len == 0) {
			vtparse(this->parser, this->io->getc());
		}
	}

	// TODO in fact this code must not be reached;
	assert(FALSE);
	return FALSE;
}

BOOL terminal_transmit(TERMINAL *this, TERMINAL_TOKEN token, int params_len,
		...) {
	va_list args;
	va_start(args, params_len);

	if (this == NULL) {
		return FALSE;
	}

	TERMINAL_TOKEN_PARAMS *params = terminal_prepare_params(this, params_len,
			args);

	va_end(args);

	VT_TOKEN *vt_token = vt_from_term_token(token, params);
	if (vt_token == NULL) {
		return FALSE;
	}
	vtbuild(this->builder, vt_token);

	return TRUE;
}
