/**
 * @file
 * @brief Terminal driver.
 * @details
 * Allows us to operate high-level tokens instead of simple chars.
 * It also provides some common ANSI/VT100 terminal control sequences.
 * This entity is backed by VTParse and VTBuild modules.
 *
 * @date 04.02.09
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <string.h>
#include <embox/kernel.h>
#include <stdarg.h>

#include <drivers/terminal.h>
#include <kernel/diag.h>

static void vtparse_callback(struct vtparse *parser, struct vt_token *);

static void vtbuild_putc(struct vtbuild *builder, char ch) {
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
	if (vtparse_init(this->parser, vtparse_callback) == NULL) {
		return NULL;
	}

	this->vt_token_queue_head = 0;
	this->vt_token_queue_len = 0;

	return this;
}

static bool is_valid_action(vt_action_t action) {
	switch (action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
	case VT_ACTION_CSI_DISPATCH:
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

static struct vt_token *vt_from_term_token(struct vt_token *vt_token,
		TERMINAL_TOKEN terminal_token, short *params, int params_len) {
	vt_action_t action = DECODE_ACTION(terminal_token);
	char char1 = DECODE_CHAR1(terminal_token);
	char char2 = DECODE_CHAR2(terminal_token);
	char code = DECODE_CODE(terminal_token);

	int attrs_len;

	if (!is_valid_action(action)) {
		return NULL;
	}

	vt_token->action = action;

	attrs_len = 0;
	if (char1) {
		vt_token->attrs[attrs_len++] = char1;
	}
	if (char2) {
		vt_token->attrs[attrs_len++] = char2;
	}
	vt_token->attrs_len = attrs_len;

	vt_token->ch = code;
	vt_token->params = params;
	vt_token->params_len = params_len;

	return vt_token;
}

static TERMINAL_TOKEN term_from_vt_token(struct vt_token *vt_token) {
	vt_action_t action;
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

static void vtparse_callback(struct vtparse *parser, struct vt_token *token) {
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
		short **params, int *params_len) {
	int *p_len, *p_head;
	struct vt_token *vt_token;
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
				if (params != NULL && params_len) {
					*params     = vt_token->params;
					*params_len = vt_token->params_len;
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

bool terminal_transmit(TERMINAL *terminal, TERMINAL_TOKEN token,
						short *params, int params_len) {
	struct vt_token vt_token;

	if (NULL == vt_from_term_token(&vt_token, token, params, params_len)) {
		return false;
	}
	vtbuild(terminal->builder, &vt_token);

	return true;
}

bool terminal_transmit_va(TERMINAL *terminal, TERMINAL_TOKEN token, int params_len,
		...) {
	va_list args;
	size_t i;
	short params[params_len];

	if (terminal == NULL) {
		return false;
	}

	va_start(args, params_len);

	if (params_len < 0) {
		params_len = 0;
	}
	for (i = 0; i < params_len; ++i) {
		params[i] = va_arg(args, int);
	}

	va_end(args);

	return terminal_transmit(terminal, token, params, params_len);
}
