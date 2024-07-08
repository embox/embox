/**
 * @file
 * @brief Core dependency resolution.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <lib/libds/array.h>
#include <framework/mod/api.h>
#include <framework/mod/ops.h>
#include <framework/mod/types.h>

#define MOD_FLAG_ENABLED       (1 << 0)

#define MOD_FLAG_OPINPROGRESS  (1 << 2)
// TODO unused for now... -- Eldar
#define MOD_FLAG_OPFAILED      (0 << 1)

#define APP_DATA_RESERVE_OFFSET ({ \
		extern char _app_reserve_vma, _app_data_vma;   \
		(size_t) (&_app_reserve_vma - &_app_data_vma); \
	})

#define mod_flag_tst(mod, mask)   ((mod)->priv->flags &   (mask))
#define mod_flag_cmp(mod, mask) (~((mod)->priv->flags ^   (mask)))
#define mod_flag_tgl(mod, mask) do (mod)->priv->flags ^=  (mask); while (0)
#define mod_flag_set(mod, mask) do (mod)->priv->flags |=  (mask); while (0)
#define mod_flag_clr(mod, mask) do (mod)->priv->flags &= ~(mask); while (0)

ARRAY_SPREAD_DEF_TERMINATED(const struct mod *const, __mod_registry, NULL);
ARRAY_SPREAD_DEF_TERMINATED(const struct mod_sec_label *const, __mod_sec_labels, NULL);

bool mod_is_running(const struct mod *mod) {
	assert(mod);
	return mod_flag_tst(mod, MOD_FLAG_ENABLED);
}

static int invoke_member_init(const struct mod_member *member) {
	const struct mod_member_ops *ops;
	mod_member_op_t init;

	if (!(ops = member->ops) || !(init = ops->init)) {
		return 0;
	}

	return init(member);
}

static int invoke_member_fini(const struct mod_member *member) {
	const struct mod_member_ops *ops;
	mod_member_op_t fini;

	if (!(ops = member->ops) || !(fini = ops->fini)) {
		return 0;
	}

	return fini(member);
}

static void mod_init_app(const struct mod *mod) {
	int i;
	const struct mod_app *app = mod->app;

	if (app) {
		/* */
		if (app->data_vma == app->data_lma) {
			/* No need to traverse deps here, they are already initialized. */
			memcpy(app->data_vma + APP_DATA_RESERVE_OFFSET, app->data_vma, app->data_sz);

			for (i = 0; app->build_dep_data_vma[i]; i++) {
				assert(app->build_dep_data_vma[i] == app->build_dep_data_lma[i]);

				memcpy(app->build_dep_data_vma[i] + APP_DATA_RESERVE_OFFSET,
						app->build_dep_data_vma[i], app->build_dep_data_sz[i]);
			}
		} else {
			/* No need to traverse deps here, they are already initialized. */
			memcpy(app->data_vma, app->data_lma, app->data_sz);

			for (i = 0; app->build_dep_data_vma[i]; i++) {
				memcpy(app->build_dep_data_vma[i], app->build_dep_data_lma[i],
						app->build_dep_data_sz[i]);
			}
		}
	}
}

int mod_activate_app(const struct mod *mod) {
	const struct mod_app *app;
	struct __mod_private priv;

	if (!mod_is_running(mod))
		return -ENOENT;

	app = mod->app;
	if (app) {
		int i;
		const struct mod *dep;

		mod_foreach_depends(dep, mod) {
			int ret = mod_activate_app(dep);
			if (ret)
				return ret;
		}

		priv = *mod->priv;
		if (app->data_vma == app->data_lma) {
			memcpy(app->data_vma, app->data_vma + APP_DATA_RESERVE_OFFSET, app->data_sz);
		} else {
			memcpy(app->data_vma, app->data_lma, app->data_sz);
		}
		memset(app->bss, 0, app->bss_sz);
		*mod->priv = priv;

		if (app->data_vma == app->data_lma) {
			for (i = 0; app->build_dep_data_vma[i]; i++) {
				memcpy(app->build_dep_data_vma[i],
						app->build_dep_data_vma[i] + APP_DATA_RESERVE_OFFSET,
						app->build_dep_data_sz[i]);
			}
		} else {
			for (i = 0; app->build_dep_data_vma[i]; i++) {
				memcpy(app->build_dep_data_vma[i], app->build_dep_data_lma[i],
						app->build_dep_data_sz[i]);
			}
		}

		for (i = 0; app->build_dep_bss[i]; i++) {
			memset(app->build_dep_bss[i], 0, app->build_dep_bss_sz[i]);
		}
	}

	return 0;
}

const struct mod *mod_lookup(const char *fqn) {
	const struct mod *mod;
	const char *mod_nm = strrchr(fqn, '.');
	size_t pkg_name_len;

	if (!mod_nm) {
		mod_nm = fqn;  /* no package, name starts from the beginning */
	}

	pkg_name_len = mod_nm - fqn;

	if (pkg_name_len) {
		++mod_nm;  /* skip '.' */
	}

	mod_foreach(mod) {
		if (strcmp(mod_name(mod), mod_nm)) {
			continue;
		}
		if (strncmp(mod_pkg_name(mod), fqn, pkg_name_len) ||
		    mod_pkg_name(mod)[pkg_name_len]) {
			continue;
		}
		return mod;
	}

	return NULL;
}

int mod_enable(const struct mod *mod) {
	const struct mod_member *member;

	if (mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	mod_init_app(mod);

	array_spread_nullterm_foreach(member, mod->members) {
		if (0 != invoke_member_init(member)) {
			goto opfailed;
		}
	}

	if (mod->ops && mod->ops->enable) {
		if (mod->ops->enable(mod)) {
			goto opfailed;
		}
	}

	mod_flag_tgl(mod, MOD_FLAG_ENABLED);

	return 0;
opfailed:
	mod_flag_set(mod, MOD_FLAG_OPFAILED);
	return -EINTR;
}

int mod_disable(const struct mod *mod) {
	const struct mod_member *member;

	if (!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	if (mod->ops && mod->ops->disable) {
		if (mod->ops->disable(mod)) {
			goto opfailed;
		}
	}

	array_spread_nullterm_foreach(member, mod->members) {
		if (0 != invoke_member_fini(member)) {
			goto opfailed;
		}
	}

	mod_flag_clr(mod, MOD_FLAG_OPFAILED);
	mod_flag_tgl(mod, MOD_FLAG_ENABLED);
	return 0;

opfailed:
	mod_flag_set(mod, MOD_FLAG_OPFAILED);
	return -EINTR;
}

int mod_enable_rec_safe(const struct mod *mod, bool rec_safe) {
	return mod_enable(mod);
}
