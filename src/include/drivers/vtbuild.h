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

struct vtbuild;

typedef void (*vtbuild_callback_t)(struct vtbuild *, char ch);

struct vtbuild {
	vtbuild_callback_t cb;
	void *user_data;
};

struct vtbuild *vtbuild_init(struct vtbuild *builder, vtbuild_callback_t cb);

void vtbuild(struct vtbuild *builder, const VT_TOKEN *token);

#endif /* VTBUILD_H_ */
