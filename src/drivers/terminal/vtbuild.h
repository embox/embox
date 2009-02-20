/*
 * vtbuild.h
 *
 *  Created on: 20.02.2009
 *      Author: Eldar Abusalimov
 */

#ifndef VTBUILD_H_
#define VTBUILD_H_

#include "vt.h"

typedef struct _VTBUILDER {
	void(*putc)(struct _VTBUILDER *builder, char ch);
	void* user_data;
} VTBUILDER;

void vtbuild_init(VTBUILDER *builder, void(*putc)(VTBUILDER *builder, char ch));

void vtbuild(VTBUILDER *builder, VT_ACTION action, INT_ARRAY *params, char ch);

#endif /* VTBUILD_H_ */
