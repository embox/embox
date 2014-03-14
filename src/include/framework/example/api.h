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

#include <util/member.h>

#include <framework/mod/api.h>
#include __impl_x(framework/example/types.h)

ARRAY_SPREAD_DECLARE(const struct example *, __example_registry);

#define EXAMPLE_ADD(_example_ptr) \
	ARRAY_SPREAD_DECLARE(const struct example *,               \
			__example_registry);                               \
	ARRAY_SPREAD_ADD(__example_registry, _example_ptr)

#define example_foreach(example_ptr) \
	array_spread_foreach(example_ptr, __example_registry)

extern int example_exec(const struct example *example, int argc, char **argv);

#include <util/member.h>

static inline const struct example_mod *__example_mod(const struct example *example) {
	return member_cast_out(example, const struct example_mod, example);
}

static inline const char *example_name(const struct example *example) {
	return (NULL != example) ? mod_name(&__example_mod(example)->mod) : NULL;
}

static inline const char *example_path(const struct example *example) {
	return (NULL != example) ? mod_pkg_name(&__example_mod(example)->mod) : NULL;
}

extern const struct example *example_lookup(const char *name);

#endif /* FRAMEWORK_EXAMPLE_API_H_ */
