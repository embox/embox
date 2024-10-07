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
#include <stdint.h>

#define MOD_STAT_ENABLED (1U << 0)
#define MOD_STAT_FAILED  (1U << 1)

typedef uint16_t mod_stat_t;

struct mod_ops;
struct mod_data;
struct mod_label;

struct mod {
	/* Module operations */
	const struct mod_ops *ops;

	/* Package name */
	const char *pkg_name;

	/* Module name */
	const char *mod_name;

	/* Security labels */
	const struct mod_label *label;

	/* Null-terminated array with dependency information */
	const struct mod *volatile const *depends;

	/* Modules, that this module depends on; which are dependent on this */
	const struct mod *volatile const *provides;

	/* Module runtime data */
	struct mod_data *data;
};

struct mod_ops {
	int (*enable)(const struct mod *self);
	int (*disable)(const struct mod *self);
};

struct mod_data {
	/* Module status */
	mod_stat_t stat;
};

struct mod_app {
	/* app's .data and .bss */
	char *data_vma;
	char *data_lma;
	char *bss;
	size_t data_sz;
	size_t bss_sz;

	/* .data and .bss of the modules on which app uses @BuildDepends */
	void **build_dep_data_vma;
	void **build_dep_data_lma;
	void **build_dep_bss;
	unsigned long *build_dep_data_sz;
	unsigned long *build_dep_bss_sz;
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

#endif /* FRAMEWORK_MOD_TYPES_H_ */
