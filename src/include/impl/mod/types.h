/**
 * @file
 * @brief Implementation private types used by mods infrastructure.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_MOD_TYPES_H_
#define IMPL_MOD_TYPES_H_

struct mod;
struct mod_package;
struct __mod_info;
struct __mod_private;

struct mod {
	/** Internal data needed by dependency resolver. */
	struct __mod_private *private;
	/** (optional) Interface with mods framework and others. */
	const struct __mod_info *info;
	/** Module package assigned by EMBuild. */
	const struct mod_package *package;
	/** Module name assigned by EMBuild. */
	const char *name;
	/** Human-readable module description. */
	const char *brief, *details;
	/** Null-terminated array containing dependency info. */
	const struct mod **requires, **provides;
};

struct mod_package {
	const char *name;
};

struct __mod_private {
	unsigned int flags;
};

#endif /* IMPL_MOD_TYPES_H_ */
