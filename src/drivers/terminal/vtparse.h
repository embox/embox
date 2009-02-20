/*
 * VTParse - an implementation of Paul Williams' DEC compatible state machine parser
 *
 * Author: Joshua Haberman <joshua@reverberate.org>
 *
 * This code is in the public domain.
 */

#ifndef VTPARSE_H_
#define VTPARSE_H_

#include "vt.h"
#include "vtparse_table.h"

#define MAX_INTERMEDIATE_CHARS 2

struct _VTPARSER;

typedef void (*VTPARSE_CALLBACK)(struct _VTPARSER*, VT_ACTION, char);

typedef struct _VTPARSER {
	VTPARSE_STATE state;
	VTPARSE_CALLBACK cb;
	char(*getc)(struct _VTPARSER *parser);
	char intermediate_chars[MAX_INTERMEDIATE_CHARS + 1];
	char ignore_flagged;
	INT_ARRAY params[1];
	void* user_data;
} VTPARSER;

void vtparse_init(VTPARSER *parser, VTPARSE_CALLBACK cb, char(*getc)(
		VTPARSER *parser));

void vtparse(VTPARSER *parser);

#endif /* VTPARSE_H_ */
