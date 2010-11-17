/**
 * @file
 * @brief VTParse - an implementation of Paul Williams'
 * DEC compatible state machine parser
 * This code is in the public domain.
 *
 * @date 04.02.2009
 * @author Joshua Haberman <joshua@reverberate.org>
 * @author Eldar Abusalimov
 *         - adaptation for using with terminal driver
 *     As we use only Plain, ESC and CSI-based tokens all the others are ignored
 *     (in spite of the fact that the Parser is capable to handle them correctly).
 */
#ifndef VTPARSE_H_
#define VTPARSE_H_

#include <drivers/vt.h>
#include <drivers/vtparse_table.h>
#include <types.h>

struct vtparse;

typedef void (*vtparse_callback_t)(struct vtparse*, VT_TOKEN*);

typedef struct vtparse {
	VTPARSE_STATE state;
	vtparse_callback_t cb;
	VT_TOKEN token[1];
	int params[VT_TOKEN_MAX_PARAMS];
	void* user_data;
} VTPARSER;

struct vtparse *vtparse_init(struct vtparse *parser, vtparse_callback_t cb);

void vtparse(struct vtparse *parser, unsigned char ch);

#endif /* VTPARSE_H_ */
