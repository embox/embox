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
struct mod_info;
struct mod_ops;
struct mod_member;
struct mod_member_ops;
struct __mod_private;

struct mod {
	/** Null-terminated array with dependency information. */
	const struct mod **requires, **provides;

	/* Descriptive information about the module provided by Embuild. */

	const struct mod_package *package; /**< Definition package. */
	const char *name;                  /**< Name assigned by EMBuild. */
	const char *brief, *details;       /**< Human-readable description. */

	/* Data used to properly enable/disable the module itself. */

	const struct mod_info    *info;    /**< (optional) Mod-specific. */
	const struct mod_member **members; /**< Members to setup/finalize. */
	struct __mod_private     *private; /**< Used by dependency resolver. */

};

struct mod_package {
	const char *name; /**< Package name assigned by EMBuild. */
};

/**
 * Mods framework manages each mod through this special interface.
 */
struct mod_info {
	void                 *data; /**< (optional) Module specific data. */
	const struct mod_ops *ops;  /**< (optional) Available operations. */
};

struct mod_member {
	void                        *data; /**< (optional) Member specific data. */
	const struct mod_member_ops *ops;  /**< (optional) Available operations. */
};

struct __mod_private {
	unsigned int flags;
};

#endif /* FRAMEWORK_MOD_TYPES_H_ */
