/**
 * @file
 * @brief Internal implementation of cmd self definition macros.
 *
 * @date Mar 1, 2011
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_SELF_IMPL_H_
#define FRAMEWORK_CMD_SELF_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <mod/self.h>

#include "types.h"

#define __EMBOX_CMD(_exec) \
	static int _exec(int argc, char **argv);           \
	ARRAY_SPREAD_ADD_NAMED(__cmd_registry, __cmd,   {  \
			.exec = _exec,                             \
			.mod = &mod_self                           \
		});                                            \
	MOD_SELF_BIND(__cmd, NULL) /* TODO not used. -- Eldar */

extern const struct cmd __cmd_registry[];

#endif /* FRAMEWORK_CMD_SELF_IMPL_H_ */
