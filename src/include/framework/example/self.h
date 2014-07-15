/**
 * @file
 * @brief TODO
 *
 * @date 01.03.11
 * @author Anton Bondarev
 */

#ifndef FRAMEWORK_EXAMPLE_SELF_H_
#define FRAMEWORK_EXAMPLE_SELF_H_

#include <util/array.h>
#include <framework/mod/self.h>
#include <framework/mod/ops.h>

#include <framework/example/api.h>

#include __impl_x(framework/example/types.h) /* TODO this is external API header */

#define EMBOX_EXAMPLE(_exec)                            \
	extern const struct mod_ops __example_mod_ops;      \
	static int _exec(int argc, char **argv);            \
	MOD_SELF_INIT_DECLS(__EMBUILD_MOD__);               \
	struct example_mod mod_self = {                     \
		/* .mod     = */ MOD_SELF_INIT(__EMBUILD_MOD__, \
				&__example_mod_ops),                    \
		/* .example = */ __EMBOX_EXAMPLE_INIT(_exec),   \
	};                                                  \
	EXAMPLE_ADD(&mod_self.example)

#define __EMBOX_EXAMPLE_INIT(_exec) { /* .exec = */ _exec }


#ifdef __CDT_PARSER__

# undef  EMBOX_EXAMPLE
# define EMBOX_EXAMPLE(function_nm) \
	static int function_nm(int, char **); \
	typedef typeof(function_nm) __example_placeholder; \
	static int function_nm(int, char **)

#endif /* __CDT_PARSER__ */

#endif /* FRAMEWORK_EXAMPLE_SELF_H_ */
