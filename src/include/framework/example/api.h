/**
 * @file
 *
 * @brief
 *
 * @date 06.07.2011
 * @author Anton Bondarev
 */

#ifndef FRAMEWORK_EXAMPLE_API_H_
#define FRAMEWORK_EXAMPLE_API_H_

#include <framework/mod/api.h>
#include __impl_x(framework/example/types.h)


extern const struct example __example_registry[];

#define example_foreach(example_ptr) \
	array_foreach_ptr(example_ptr, __example_registry, \
			ARRAY_SPREAD_SIZE(__example_registry))

extern int example_exec(const struct example *example, int argc, char **argv);

static inline const char *example_name(const struct example *example) {
	return (NULL != example) ? example->mod->name : NULL;
}

static inline const char *example_path(const struct example *example) {
	return (NULL != example) ? example->mod->package->name +
	                         sizeof("embox.example") : NULL;
}

extern const struct example *example_lookup(const char *name);

#endif /* FRAMEWORK_EXAMPLE_API_H_ */
