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
	extern const struct cmd __cmd_registry[];        \
	static int _exec(int argc, char **argv);         \
	ARRAY_SPREAD_ADD_NAMED(__cmd_registry, __cmd, {  \
			.exec = _exec,                           \
			.mod = &mod_self                         \
		});                                          \
	extern const struct mod_ops __cmd_mod_ops;       \
	MOD_INFO_BIND(&__cmd_mod_ops, __cmd)


#ifdef __CDT_PARSER__

# undef  __EMBOX_CMD
# define __EMBOX_CMD(function_nm) \
	static int function_nm(int, char **); \
	typedef typeof(function_nm) __cmd_placeholder; \
	static int function_nm(int, char **)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_CMD_SELF_IMPL_H_ */
