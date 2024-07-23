/**
 * @file
 * @brief TODO
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_SELF_H_
#define FRAMEWORK_CMD_SELF_H_

#include <sys/cdefs.h>

#include <framework/cmd/api.h>
#include <framework/cmd/types.h>
#include <framework/mod/self.h>
#include <lib/libds/array.h>
#include <util/macro.h>

#define _EMBOX_CMD_DEF(_exec) __EMBOX_CMD_DEF(_exec, MACRO_GUARD(__cmd_mod))

/* clang-format off */
#define __EMBOX_CMD_DEF(_exec, _cmd_mod)            \
	const struct cmd_mod __MOD(__EMBUILD_MOD__) = { \
	    .mod = MOD_SELF_INIT(&__cmd_mod_ops),       \
	    .app = &__MOD_APP(__EMBUILD_MOD__),         \
	    .cmd = {                                    \
	        .exec = _exec,                          \
	        .desc = &__MOD_CMD(__EMBUILD_MOD__),    \
		},                                          \
	};                                              \
	CMD_REGISTER(&__MOD(__EMBUILD_MOD__).cmd);      \
	MOD_SELF_RUNTIME()
/* clang-format on */

#define __EMBOX_EXTERN_CMD(_exec)            \
	extern int _exec(int argc, char **argv); \
	_EMBOX_CMD_DEF(_exec)

#define EMBOX_CMD(_exec)                     \
	static int _exec(int argc, char **argv); \
	_EMBOX_CMD_DEF(_exec)

#ifdef __CDT_PARSER__

#undef EMBOX_CMD
#define EMBOX_CMD(function_nm)                     \
	static int function_nm(int, char **);          \
	typedef typeof(function_nm) __cmd_placeholder; \
	static int function_nm(int, char **)

#endif /* __CDT_PARSER__ */

__BEGIN_DECLS

extern const struct mod_ops __cmd_mod_ops;

extern const struct cmd_desc __MOD_CMD(__EMBUILD_MOD__);
extern const struct mod_app __MOD_APP(__EMBUILD_MOD__) __attribute__((weak));

__END_DECLS

#endif /* FRAMEWORK_CMD_SELF_H_ */
