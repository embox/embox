/**
 * @file
 * @brief Terminal driver.
 * @details
 * Allows us to operate high-level tokens instead of simple chars.
 * It also provides some common ANSI/VT100 terminal control sequences.
 * This entity is backed by VTParse and VTBuild modules.
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <string.h>
#include <embox/kernel.h>
#include <stdarg.h>

#include <drivers/terminal.h>
#include <kernel/diag.h>

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
	if(vtparse_init(this->parser, vtparse_callback) == NULL) {
		return NULL;
	}

	this->vt_token_queue_head = 0;
	this->vt_token_queue_len = 0;

	return this;
}

static bool is_valid_action(VT_ACTION action) {
	switch (action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
	case VT_ACTION_CS_DISPATCH:
	case VT_ACTION_ESC_DISPATCH:
		/* ok */
		return true;
	case VT_ACTION_OSC_START:
	case VT_ACTION_OSC_PUT:
	case VT_ACTION_OSC_END:
		/* Operating System Command */
		/* ignore them as unused in our system
		  -- Eldar*/
	case VT_ACTION_HOOK:
	case VT_ACTION_PUT:
	case VT_ACTION_UNHOOK:
		/* device control strings */
		/* ignore them as unused in our system
		  -- Eldar*/
	case VT_ACTION_IGNORE:
	case VT_ACTION_COLLECT:
	case VT_ACTION_PARAM:
	case VT_ACTION_CLEAR:
		/* ignore as VTParser internal states */
	default:
		/* unknown */
		return false;
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
	VT_ACTION action;
	char *a;
	int a_len;
	char char1, char2, code;
	if (vt_token == NULL) {
		return TERMINAL_TOKEN_EMPTY;
	}

	action = vt_token->action;
	if (!is_valid_action(action)) {
		return TERMINAL_TOKEN_EMPTY;
	}

	a = vt_token->attrs;
	a_len = vt_token->attrs_len;
	char1 = (a_len > 0) ? a[0] : '\0';
	char2 = (a_len > 1) ? a[1] : '\0';
	code = vt_token->ch;

	return ENCODE(action,char1,char2, code);
}

static TERMINAL_TOKEN_PARAMS *terminal_prepare_params(TERMINAL *this,
		int length, va_list args) {
	TERMINAL_TOKEN_PARAMS *params = this->default_params;
	int i;
	if (length < 0) {
		length = 0;
	}
	params->length = length;
	for (i = 0; i < length; ++i) {
		params->data[i] = va_arg(args, int);
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
		/* TODO*/
		assert(false);
	}
}
#if 0
static char *ACTION_NAMES[] = { "<no action>", "CLEAR", "COLLECT", "CSI_DISPATCH",
		"ESC_DISPATCH", "EXECUTE", "HOOK", "IGNORE", "OSC_END", "OSC_PUT",
		"OSC_START", "PARAM", "PRINT", "PUT", "UNHOOK", };
#endif

/*
 * Yes, this routine is rather ugly, but I have not found another way
 * to implement synchronous token receiving
 * without involving parser callback
 *
 * TODO describe this shit
 */
bool terminal_receive(TERMINAL *this, TERMINAL_TOKEN *token,
		TERMINAL_TOKEN_PARAMS *params) {
	int *p_len, *p_head;
	VT_TOKEN *vt_token;
	if (this == NULL) {
		return false;
	}

	p_len = &this->vt_token_queue_len;
	p_head = &this->vt_token_queue_head;
	while (*p_len == 0) {
		/* we don't return from vtparse until
		 callback has not been invoked at least once.*/
		vtparse(this->parser, this->io->getc());
		/* so vt_token_queue_len is greater then 0 at this line
		 (see vtparse_callback algorithm)*/
	}
	assert(*p_len != 0);

	/* Almost surely this loop will be executed only once */
	while (*p_len != 0) {
		for (; *p_head < *p_len; (*p_head)++) {
			vt_token = &this->vt_token_queue[*p_head];
			if ((*token = term_from_vt_token(vt_token)) != TERMINAL_TOKEN_EMPTY) {
				if (params != NULL) {
					*params->data = *vt_token->params;
					params->length = vt_token->params_len;
				}
				(*p_len)--;

				return true;
			}
		}
		/* there were no any valid tokens in the queue,
		 so reset queue head and length indexes
		 and run parser again
		 in general case this code should not be reached */
		*p_head = 0;
		*p_len = 0;
		while (*p_len == 0) {
			vtparse(this->parser, this->io->getc());
		}
	}

	/* TODO in fact this code must not be reached;*/
	assert(false);
	return false;
}

bool terminal_transmit(TERMINAL *this, TERMINAL_TOKEN token, int params_len,
		...) {
	va_list args;
	TERMINAL_TOKEN_PARAMS *params;
	VT_TOKEN *vt_token;
	va_start(args, params_len);

	if (this == NULL) {
		return false;
	}

	params = terminal_prepare_params(this, params_len, args);

	va_end(args);

	vt_token = vt_from_term_token(token, params);
	if (vt_token == NULL) {
		return false;
	}
	vtbuild(this->builder, vt_token);

	return true;
}
