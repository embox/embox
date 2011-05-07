/**
 * @file
 * @brief Implementation private types used by mods infrastructure.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_TYPES_H_
#define FRAMEWORK_MOD_TYPES_H_

struct mod;
struct mod_package;
struct mod_ops;
struct __mod_info;
struct __mod_private;

struct mod {
	/** Null-terminated array with dependency information. */
	const struct mod **requires, **provides;

	const struct mod_package *package; /**< Package assigned by EMBuild. */
	const struct __mod_info  *info;    /**< (optional) Mod-specific data. */
	struct __mod_private     *private; /**< Needed by dependency resolver. */

	const char *name;                  /**< Name assigned by EMBuild. */
	const char *brief, *details;       /**< Human-readable description. */

};

struct mod_package {
	const char *name; /**< Package name assigned by EMBuild. */
};

/**
 * Mods framework manages each mod through this special interface.
 */
struct __mod_info {
	void           *data; /**< (optional) Module specific data. */
	struct mod_ops *ops;  /**< (optional) Available operations. */
};

struct __mod_private {
	unsigned int flags;
};

#endif /* FRAMEWORK_MOD_TYPES_H_ */
