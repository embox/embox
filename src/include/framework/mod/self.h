/**
 * @file
 * @brief Self mod bindings for #mod_info.
 * @details
 *   Self mod is the one that is associated with the current compilation unit.
 *   Bindings associate application-specific operations and data with the self
 *   mod. Such information is then used to properly initialize and finalize
 *   the @link #mod_info module @endlink.
 *
 * @note
 *   Do not include from any source which is not a part of some mod.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 *          - Initial implementation for mod_info
 */

#ifndef FRAMEWORK_MOD_SELF_H_
#define FRAMEWORK_MOD_SELF_H_

#include <sys/cdefs.h>

#include <framework/mod/decls.h>
#include <framework/mod/embuild.h>
#include <framework/mod/runlevel.h>

/* Initialize current module */
#define MOD_SELF_INIT(mod_ops) _MOD_INIT(__EMBUILD_MOD__, mod_ops)

/* Make current module a runtime module */
#define MOD_SELF_RUNTIME()                       \
	struct mod_data __MOD_DATA(__EMBUILD_MOD__); \
	RUNLEVEL_MOD_REGISTER(__EMBUILD_MOD__)

__BEGIN_DECLS

/* Required for MOD_SELF_INIT() */
extern struct mod_data __MOD_DATA(__EMBUILD_MOD__) __attribute__((weak));
extern const struct mod_label __MOD_LABEL(__EMBUILD_MOD__)
    __attribute__((weak));

__END_DECLS

/* Required for MOD_SELF_INIT() */
ARRAY_SPREAD_DECLARE(const struct mod *, __MOD_DEPENDS(__EMBUILD_MOD__))
__attribute__((weak));
ARRAY_SPREAD_DECLARE(const struct mod *, __MOD_PROVIDES(__EMBUILD_MOD__))
__attribute__((weak));

#endif /* FRAMEWORK_MOD_SELF_H_ */
