/**
 * @file
 * @brief VTBuild - Provides VT token printing routines as opposed to VTParse module
 * @details As we use only Plain, ESC and CSI-based tokens
 * all the others are ignored (the same remark concerns to VTParse too).
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */

#include <types.h>

#include <drivers/vtbuild.h>
#include <embox/kernel.h>
#include <util/math.h>

/* Enough to hold decimal numbers up to 99999. */
#define PARAM_PRINT_BUFF_SIZE 5

/** ANSI Escape */
#define ESC		'\033'

/** ANSI Control Sequence Introducer */
#define CSI		'['

struct vtbuild *vtbuild_init(struct vtbuild *builder, vtbuild_callback_t cb) {
	if (builder == NULL) {
		return NULL;
	}

	builder->cb = cb;
	return builder;
}

static void build_single_param(struct vtbuild *builder, short n) {
	char buff[PARAM_PRINT_BUFF_SIZE];
	int i = 0;

	if (n < 0) {
		return;
	}

	do {
		buff[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	do {
		builder->cb(builder, buff[--i]);
	} while (i > 0);
}

static void build_params(struct vtbuild *builder, const short *params,
		int params_len) {
	size_t i;
	if (params != NULL) {
		if (params_len > 0) {
			build_single_param(builder, params[0]);
		}
		for (i = 1; i < params_len; i++) {
			builder->cb(builder, ';');
			build_single_param(builder, params[i]);
		}
	}
}

static void build_attrs(struct vtbuild *builder, const char *attrs,
		int attrs_len) {
	size_t i;
	if (attrs != NULL) {
		for (i = 0; i < attrs_len; i++) {
			builder->cb(builder, attrs[i]);
		}
	}
}

void vtbuild(struct vtbuild *builder, const struct vt_token *token) {

	switch (token->action) {
	case VT_ACTION_EXECUTE:
	case VT_ACTION_PRINT:
		builder->cb(builder, token->ch);
		break;

	case VT_ACTION_ESC_DISPATCH:
		builder->cb(builder, ESC);
		/* TODO check, may be the following two lines should be swapped. -- Eldar*/
		builder->cb(builder, token->ch);
		build_attrs(builder, token->attrs,
				min(token->attrs_len, VT_TOKEN_ATTRS_MAX));
		break;

	case VT_ACTION_CSI_DISPATCH:
		builder->cb(builder, ESC);
		builder->cb(builder, CSI);
		build_params(builder, token->params, token->params_len);
		build_attrs(builder, token->attrs,
				min(token->attrs_len, VT_TOKEN_ATTRS_MAX));
		builder->cb(builder, token->ch);
		break;

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
