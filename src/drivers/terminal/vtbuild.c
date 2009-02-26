/*
 * vtbuild.c
 *
 *  Created on: 20.02.2009
 *      Author: Eldar Abusalimov
 */

#include "vtbuild.h"

#include "vt.h"
#include "types.h"
#include "common.h"

void vtbuild_init(VTBUILDER *builder, void(*putc)(VTBUILDER *builder, char ch)) {
	builder->putc = putc;
}

static void build_param(VTBUILDER *builder, int n) {
	static char buf[10];
	int i = 0;
	BOOL neg = (n < 0);

	n = abs(n);

	do {
		buf[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (neg) {
		builder->putc(builder, '-');
	}
	do {
		builder->putc(builder, buf[--i]);
	} while (i > 0);
}

void vtbuild(VTBUILDER *builder, VT_ACTION action, VT_PARAMS *params, char ch) {
	switch (action) {
	case VT_ACTION_PRINT:
		builder->putc(builder, ch);
		break;
	case VT_ACTION_ESC_DISPATCH:
		builder->putc(builder, ESC);
		builder->putc(builder, ch);
		break;
	case VT_ACTION_CSI_DISPATCH:
		builder->putc(builder, ESC);
		builder->putc(builder, CSI);
		if (params != NULL) {
			int i;
			char d;
			if (params->length > 0) {
				build_param(builder, params->data[0]);
			}
			for (i = 1; i < params->length; i++) {
				builder->putc(builder, ';');
				build_param(builder, params->data[i]);
			}
		}
		builder->putc(builder, ch);
		break;
	case VT_ACTION_EXECUTE:
	case VT_ACTION_HOOK:
	case VT_ACTION_PUT:
	case VT_ACTION_OSC_START:
	case VT_ACTION_OSC_PUT:
	case VT_ACTION_OSC_END:
	case VT_ACTION_UNHOOK:
	default:
		TRACE("Not implemented");
		break;
	}
}
