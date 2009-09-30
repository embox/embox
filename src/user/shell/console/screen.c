/*
 * screen.c
 *
 *  Created on: 28.02.2009
 *      Author: Eldar Abusalimov
 */

#include "screen.h"

#include "cmdline.h"
#include "asm/types.h"

SCREEN * screen_init(SCREEN *this, SCREEN_IO *io) {
	if (this == NULL || io == NULL) {
		return NULL;
	}

	if (terminal_init(this->terminal, io) == NULL) {
		return NULL;
	}

	return this;
}

#include "screen_in.c"
#include "screen_out.c"
