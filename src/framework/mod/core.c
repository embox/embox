/**
 * @file
 * @brief Core dependency resolution.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <framework/mod/api.h>
#include <framework/mod/types.h>
#include <lib/libds/array.h>

bool mod_is_running(const struct mod *mod) {
	assert(mod);

	return mod->data ? (mod->data->stat & MOD_STAT_ENABLED) : false;
}

const struct mod *mod_lookup(const char *fqn) {
	const struct mod *mod;
	const char *mod_nm = strrchr(fqn, '.');
	size_t pkg_name_len;

	if (!mod_nm) {
		mod_nm = fqn; /* no package, name starts from the beginning */
	}

	pkg_name_len = mod_nm - fqn;

	if (pkg_name_len) {
		++mod_nm; /* skip '.' */
	}

	mod_foreach(mod) {
		if (strcmp(mod_name(mod), mod_nm)) {
			continue;
		}
		if (strncmp(mod_pkg_name(mod), fqn, pkg_name_len)
		    || mod_pkg_name(mod)[pkg_name_len]) {
			continue;
		}
		return mod;
	}

	return NULL;
}

int mod_enable(const struct mod *mod) {
	int err;

	if (mod_is_running(mod)) {
		return 0;
	}

	if (mod->ops && mod->ops->enable) {
		if ((err = mod->ops->enable(mod))) {
			mod->data->stat |= MOD_STAT_FAILED;
			return err;
		}
		mod->data->stat |= MOD_STAT_ENABLED;
	}

	return 0;
}

int mod_disable(const struct mod *mod) {
	int err;

	if (!mod_is_running(mod)) {
		return 0;
	}

	if (mod->ops && mod->ops->disable) {
		if ((err = mod->ops->disable(mod))) {
			mod->data->stat |= MOD_STAT_FAILED;
			return err;
		}
		mod->data->stat &= ~(mod_stat_t)MOD_STAT_ENABLED;
	}

	return 0;
}

int mod_enable_rec_safe(const struct mod *mod, bool rec_safe) {
	return mod_enable(mod);
}
