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
#include <drivers/vtparse_state.h>
#include <types.h>

#define VTPARSE_TOKEN_PARAMS_MAX    8

struct vtparse;

typedef void (*vtparse_callback_t)(struct vtparse *, struct vt_token *);

struct vtparse {
	vtparse_state_t state;
	vtparse_callback_t cb;
	struct vt_token token;
	short token_params[VTPARSE_TOKEN_PARAMS_MAX];
	void* user_data;
};

typedef struct vtparse *vtparse_t;

struct vtparse *vtparse_init(struct vtparse *parser, vtparse_callback_t cb);

void vtparse(struct vtparse *parser, unsigned char ch);

#endif /* VTPARSE_H_ */
