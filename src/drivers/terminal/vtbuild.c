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

void vtbuild(VTBUILDER *builder, VT_ACTION action, INT_ARRAY *params, char ch) {
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
			for (i = 0; i < params->length; i++) {
				// TODO print params
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
