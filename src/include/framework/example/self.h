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

#include __impl_x(framework/example/types.h) /* TODO this is external API header */

/* TODO this lines may place not in macro */
extern const struct example __example_registry[];
extern const struct mod_ops __example_mod_ops;

#define EMBOX_EXAMPLE(_exec)                                   \
	static int _exec(int argc, char **argv);                   \
	ARRAY_SPREAD_ADD_NAMED(__example_registry, __example,   {  \
			.mod = &mod_self,                                  \
			.exec = _exec,                                     \
		});                                                    \
	MOD_INFO_BIND(&__example_mod_ops, __example)


#endif /* FRAMEWORK_EXAMPLE_SELF_H_ */
