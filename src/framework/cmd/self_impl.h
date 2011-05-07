/**
 * @file
 * @brief Internal implementation of cmd self definition macros.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_SELF_IMPL_H_
#define FRAMEWORK_CMD_SELF_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <framework/mod/self.h>

#include "types.h"

#define __EMBOX_CMD(_exec) \
	extern const struct cmd __cmd_registry[];          \
	static int _exec(int argc, char **argv);           \
	ARRAY_SPREAD_ADD_NAMED(__cmd_registry, __cmd,   {  \
			.exec = _exec,                     \
			.mod = &mod_self                   \
		});                                        \
	MOD_SELF_BIND(__cmd, NULL) /* TODO not used. -- Eldar */

#ifdef __CDT_PARSER__

# undef __EMBOX_CMD
# define __EMBOX_CMD(_exec) \
	static int _exec(int, char **)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_CMD_SELF_IMPL_H_ */
