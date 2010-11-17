/**
 * @file
 * @brief VTBuild - Provides VT token printing routines as opposed to VTParse module
 * @details As we use only Plain, ESC and CSI-based tokens
 * all the others are ignored (the same remark concerns to VTParse too).
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */
#include <drivers/vtbuild.h>
#include <embox/kernel.h>
#include <types.h>

#define BUF_SIZE 10

VTBUILDER * vtbuild_init(VTBUILDER *this, void(*putc)(VTBUILDER *builder,
		char ch)) {
	if (this == NULL) {
		return NULL;
	}

	this->putc = putc;
	return this;
}

static void build_param(VTBUILDER *this, int n) {
	static char buf[BUF_SIZE];
	int i = 0;
	if (n < 0) {
		return;
	}

	do {
		buf[i++] = n % BUF_SIZE + '0';
	} while ((n /= BUF_SIZE) > 0);

	do {
		this->putc(this, buf[--i]);
	} while (i > 0);
}

void vtbuild(VTBUILDER *this, const VT_TOKEN *token) {
	int i;
	switch (token->action) {
	case VT_ACTION_EXECUTE:
	case VT_ACTION_PRINT:
		this->putc(this, token->ch);
		break;
	case VT_ACTION_ESC_DISPATCH:
		this->putc(this, ESC);
		this->putc(this, token->ch);
		for (i = 0; i < token->attrs_len; i++) {
			this->putc(this, token->attrs[i]);
		}
		break;
	case VT_ACTION_CSI_DISPATCH: {
		this->putc(this, ESC);
		this->putc(this, CSI);
		if (token->params != NULL) {
			if (token->params_len > 0) {
				build_param(this, token->params[0]);
			}
			for (i = 1; i < token->params_len; i++) {
				this->putc(this, ';');
				build_param(this, token->params[i]);
			}
		}
		for (i = 0; i < token->attrs_len; i++) {
			this->putc(this, token->attrs[i]);
		}
		this->putc(this, token->ch);
		break;
	}
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
		break;
	case VT_ACTION_IGNORE:
	case VT_ACTION_COLLECT:
	case VT_ACTION_PARAM:
	case VT_ACTION_CLEAR:
		/* ignore as VTParser internal states*/
		break;
	default:
		/* unknown action
		 do not fail, just ignore it*/
		break;
	}
}
