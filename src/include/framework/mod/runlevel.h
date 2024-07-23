/**
 * @file
 * @brief TODO
 *
 * @date 10.03.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_RUNLEVEL_H_
#define FRAMEWORK_MOD_RUNLEVEL_H_

#include <sys/cdefs.h>

#include <framework/mod/decls.h>
#include <lib/libds/array.h>
#include <util/macro.h>

/** Total amount of run levels. */
#define RUNLEVEL_NRS_TOTAL   5

/** Last runlevel contains modules which are included in the
  * config, but we don't want them to be started for some reason */
#define RUNLEVEL_NRS_ENABLED (RUNLEVEL_NRS_TOTAL - 1)

/**
 * Checks if the specified @c runlevel_nr is less then #RUNLEVEL_NRS_TOTAL
 * value.
 *
 * @param runlevel_nr the number to check
 * @return comparison result
 */
#define runlevel_nr_valid(runlevel_nr) \
	(0 <= (runlevel_nr_t)runlevel_nr   \
	    && (runlevel_nr_t)runlevel_nr < (runlevel_nr_t)RUNLEVEL_NRS_TOTAL)

/**
 * Iterates over a list of runtime modules contained at @a level.
 *
 * @param mod
 *   Iteration variable which takes a value of each module.
 * @param level
 *   Runievel containing modules.
 */
#define runlevel_mod_foreach(mod, level) \
	__runlevel_mod_foreach(mod, level, MACRO_GUARD(__ptr))

#define __runlevel_mod_foreach(mod, level, _ptr)                              \
	for (const struct mod *const volatile *_ptr = mod_runlevels_start[level]; \
	     (_ptr != mod_runlevels_end[level]) && ((mod = *_ptr) || 1); _ptr++)

/**
 * Iterates over a list of runtime modules contained at runlevel @a level
 * in reverse direction.
 *
 * @param mod
 *   Iteration variable which takes a value of each module.
 * @param level
 *   Runievel containing modules.
 */
#define runlevel_mod_foreach_reverse(mod, level) \
	__runlevel_mod_foreach_reverse(mod, level, MACRO_GUARD(__ptr))

#define __runlevel_mod_foreach_reverse(mod, level, _ptr)                    \
	for (const struct mod *const volatile *_ptr = mod_runlevels_end[level]; \
	     (_ptr != mod_runlevels_start[level]) && ((mod = *_ptr) || 1); _ptr--)

/**
 * Add a module @a mod to registry of modules contained at runlevel @a level.
 *
 * @param mod_nm
 */
#define RUNLEVEL_MOD_REGISTER(mod_nm)                                          \
	ARRAY_SPREAD_DECLARE(const struct mod *, __RUNLEVEL_MOD_REGISTRY(mod_nm)); \
	__ARRAY_SPREAD_ADD_ORDERED(__RUNLEVEL_MOD_REGISTRY(mod_nm),                \
	    __MOD_PRIORITY(__EMBUILD_MOD__), (const struct mod *)&__MOD(mod_nm))

#define __RUNLEVEL_MOD_REGISTRY(mod_nm) \
	MACRO_FOREACH(MACRO_CONCAT, __mod_runlevel, __MOD_RUNLEVEL(mod_nm))

/** Type representing the run level. */
typedef int runlevel_nr_t;

__BEGIN_DECLS

extern const struct mod *const volatile *mod_runlevels_start[RUNLEVEL_NRS_TOTAL];
extern const struct mod *const volatile *mod_runlevels_end[RUNLEVEL_NRS_TOTAL];

/**
 * Sets the run level to the exact value.
 *
 * @param level the new run level
 * @return operation result
 * @retval 0 on success
 * @retval nonzero TODO
 */
extern int runlevel_set(runlevel_nr_t level);

__END_DECLS

#endif /* FRAMEWORK_MOD_RUNLEVEL_H_ */
