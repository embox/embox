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
	static int _exec(int argc, char **argv);   \
	struct cmd_mod mod_self = {                \
		.mod = MOD_SELF_INIT(NULL),            \
		.cmd = {                               \
			.exec = _exec,                     \
		}                                      \
	};                                         \
	CMD_ADD(&mod_self.cmd)

#ifdef __CDT_PARSER__

# undef  __EMBOX_CMD
# define __EMBOX_CMD(function_nm) \
	static int function_nm(int, char **); \
	typedef typeof(function_nm) __cmd_placeholder; \
	static int function_nm(int, char **)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_CMD_SELF_IMPL_H_ */
