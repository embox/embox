/**
 * @file
 * @brief VTBuild - Provides VT token printing routines
 * as opposed to VTParse module
 * @details
 * As we use only Plain, ESC and CSI-based tokens
 * all the others are ignored (the same remark concerns to VTParse too).
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */
#ifndef VTBUILD_H_
#define VTBUILD_H_

#include <drivers/vt.h>

typedef struct _VTBUILDER {
	void (*putc)(struct _VTBUILDER *builder, char ch);
	void *user_data;
} VTBUILDER;

VTBUILDER *vtbuild_init(VTBUILDER *, void(*putc)(VTBUILDER *builder, char ch));

void vtbuild(VTBUILDER *, const VT_TOKEN *token);

#endif /* VTBUILD_H_ */
