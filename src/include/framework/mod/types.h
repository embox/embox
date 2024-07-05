/**
 * @file
 * @brief Implementation private types used by mods infrastructure.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_TYPES_H_
#define FRAMEWORK_MOD_TYPES_H_

#include <stddef.h>

struct mod;
struct mod_build_info;
struct __mod_private;

struct mod_ops;
struct mod_app;
struct mod_member;

struct logger;

struct mod {
	const struct mod_ops *ops;

	/**< Used by dependency resolver. */
	struct __mod_private *priv;

	/* Data used to properly enable/disable the module itself. */

	/**< (optional) Application specific. */
	const struct mod_app *app;

	/**< Members to setup/finalize. */
	const struct mod_member *volatile const *members;

	/* Const build info data */
	const struct mod_build_info *build_info;
};

struct mod_app {
	/* app's .data and .bss */
	char *data_vma;
	char *data_lma;
	size_t data_sz;
	char *bss;
	size_t bss_sz;

	/* .data and .bss of the modules on which app uses @BuildDepends */
	void **build_dep_data_vma;
	void **build_dep_data_lma;
	unsigned *build_dep_data_sz;
	void **build_dep_bss;
	unsigned *build_dep_bss_sz;
};

struct __mod_private {
	unsigned int flags;
};

struct __mod_section {
	char *vma;
	size_t len;
	const char *md5sum;
};

struct mod_label {
	struct __mod_section text;
	struct __mod_section rodata;
	struct __mod_section data;
	struct __mod_section bss;
};

struct mod_sec_label {
	struct mod_label label;
	const struct mod *mod;
};

/* Descriptive information about the module provided by Embuild. */
struct mod_build_info {
	/**< Definition package. */
	const char *pkg_name;

	/**< Name assigned by EMBuild. */
	const char *mod_name;

	/**< (optional) Security. */
	const struct mod_label *label;

	// struct logger *const logger;

	/* Null-terminated array with dependency information. */
	const struct mod *volatile const *requires;

	/**< Modules, that this module depends on; which are dependent on this. */
	const struct mod *volatile const *provides;

	/**< Should be loaded right after this. */
	const struct mod *volatile const *after_deps;

	/**< Contained in this module. */
	const struct mod *volatile const *contents;
};

#endif /* FRAMEWORK_MOD_TYPES_H_ */
