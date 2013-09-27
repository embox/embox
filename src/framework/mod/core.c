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

#include <util/array.h>
#include <framework/mod/api.h>
#include <framework/mod/ops.h>
#include <framework/mod/member/ops.h>
#include "types.h"

#define MOD_FLAG_ENABLED       (1 << 0)

#define MOD_FLAG_OPINPROGRESS  (1 << 2)
// TODO unused for now... -- Eldar
#define MOD_FLAG_OPFAILED      (0 << 1)

#define APP_DATA_RESERVE_OFFSET ({ \
		extern char __app_data_start; \
		extern char __app_reserve_start; \
		&__app_reserve_start - &__app_data_start; \
	})

#define mod_flag_tst(mod, mask)   ((mod)->priv->flags &   (mask))
#define mod_flag_cmp(mod, mask) (~((mod)->priv->flags ^   (mask)))
#define mod_flag_tgl(mod, mask) do (mod)->priv->flags ^=  (mask); while (0)
#define mod_flag_set(mod, mask) do (mod)->priv->flags |=  (mask); while (0)
#define mod_flag_clr(mod, mask) do (mod)->priv->flags &= ~(mask); while (0)

ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __mod_registry, NULL);

static int mod_traverse(const struct mod *mod,
		int (*mod_op)(const struct mod *), unsigned int flags);
static int mod_do_enable(const struct mod *mod);
static int mod_do_disable(const struct mod *mod);
static void mod_init_app(const struct mod *mod);

bool mod_is_running(const struct mod *mod) {
	assert(mod);
	return mod_flag_tst(mod, MOD_FLAG_ENABLED);
}

int mod_enable_rec_safe(const struct mod *mod, bool rec_safe) {
	unsigned int flags = MOD_FLAG_ENABLED;
	if (rec_safe)
		flags |= MOD_FLAG_OPINPROGRESS;

	assert(mod);
	return mod_traverse(mod, mod_do_enable, flags);
}

int mod_enable(const struct mod *mod) {
	return mod_enable_rec_safe(mod, false);
}

int mod_disable_rec_safe(const struct mod *mod, bool rec_safe) {
	unsigned int flags = 0;
	if (rec_safe)
		flags |= MOD_FLAG_OPINPROGRESS;

	assert(mod);
	return mod_traverse(mod, mod_do_disable, flags);
}

int mod_disable(const struct mod *mod) {
	return mod_disable_rec_safe(mod, false);
}

static int mod_traverse_all(const struct mod **mods,
		int (*mod_op)(const struct mod *), unsigned int flags) {
	const struct mod *mod;

	array_nullterm_foreach(mod, mods) {
		if (0 != mod_traverse(mod, mod_op, flags))
			return -EINTR;
	}

	return 0;
}

static int mod_traverse(const struct mod *mod,
		int (*mod_op)(const struct mod *), unsigned int flags) {
	int ret;
	int is_enable = (flags & MOD_FLAG_ENABLED);
	const struct mod **deps = (is_enable ? mod->requires : mod->provides);

	if (mod_flag_cmp(mod, flags) & MOD_FLAG_ENABLED)
		return 0;

	if (mod_flag_tst(mod, flags & MOD_FLAG_OPINPROGRESS))
		return 0;  // TODO Isn't it an error? -- Eldar

	assert(!mod_flag_tst(mod, MOD_FLAG_OPINPROGRESS),
			"Recursive mod traversal");

	mod_flag_tgl(mod, MOD_FLAG_OPINPROGRESS);

	if (!is_enable) {
		ret = mod_traverse_all(mod->after_deps, mod_op, flags);
		if (ret)
			goto out;
	}

	ret = mod_traverse_all(deps, mod_op, flags);
	if (ret)
		goto out;

	ret = mod_op(mod);
	if (ret)
		goto out;

	if (is_enable) {
		ret = mod_traverse_all(mod->after_deps, mod_op, flags);
		if (ret)
			goto out;
	}

out:
	mod_flag_tgl(mod, MOD_FLAG_OPINPROGRESS);
	return ret;
}

static inline const struct mod_ops *mod_ops_deref(const struct mod *mod) {
	const struct mod_info *info = mod->info;
	return info ? info->ops : NULL;
}

static int invoke_mod_enable(const struct mod *mod) {
	const struct mod_ops *ops;
	mod_op_t enable;

	if (!(ops = mod_ops_deref(mod)) || !(enable = ops->enable)) {
		return 0;
	}

	return enable((struct mod_info *) mod->info);
}

static int invoke_mod_disable(const struct mod *mod) {
	const struct mod_ops *ops;
	mod_op_t disable;

	if (!(ops = mod_ops_deref(mod)) || !(disable = ops->disable)) {
		return 0;
	}

	return disable((struct mod_info *) mod->info);
}

static int invoke_member_init(const struct mod_member *member) {
	const struct mod_member_ops *ops;
	mod_member_op_t init;

	if (!(ops = member->ops) || !(init = ops->init)) {
		return 0;
	}

	return init((struct mod_member *) member);
}

static int invoke_member_fini(const struct mod_member *member) {
	const struct mod_member_ops *ops;
	mod_member_op_t fini;

	if (!(ops = member->ops) || !(fini = ops->fini)) {
		return 0;
	}

	return fini((struct mod_member *) member);
}

static int mod_do_enable(const struct mod *mod) {
	const struct mod_member *member;

	if (mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	mod_init_app(mod);

	array_nullterm_foreach(member, mod->members) {
		if (0 != invoke_member_init(member)) {
			goto opfailed;
		}
	}

	if (0 != invoke_mod_enable((struct mod *) mod)) {
		goto opfailed;
	}

	mod_flag_clr(mod, MOD_FLAG_OPFAILED);
	mod_flag_tgl(mod, MOD_FLAG_ENABLED);

	return 0;

opfailed:
	mod_flag_set(mod, MOD_FLAG_OPFAILED);
	return -EINTR;
}

static int mod_do_disable(const struct mod *mod) {
	const struct mod_member *member;

	if (!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	if (0 != invoke_mod_disable((struct mod *) mod)) {
		goto opfailed;
	}

	array_nullterm_foreach(member, mod->members) {
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

static void mod_init_app(const struct mod *mod) {
	const struct mod_app *app = mod->app;

	if (app)
		memcpy(app->data + APP_DATA_RESERVE_OFFSET, app->data, app->data_sz);
}

int mod_activate_app(const struct mod *mod) {
	const struct mod_app *app;

	if (!mod_is_running(mod))
		return -ENOENT;

	app = mod->app;
	if (app) {
		memcpy(app->data, app->data + APP_DATA_RESERVE_OFFSET, app->data_sz);
		memset(app->bss, 0, app->bss_sz);
	}

	return 0;
}

const struct mod *mod_lookup(const char *fqn) {
	const struct mod *mod;
	const char *mod_name = strrchr(fqn, '.');
	size_t pkg_name_len;

	if (!mod_name) {
		mod_name = fqn;  /* no package, name starts from the beginning */
	}

	pkg_name_len = mod_name - fqn;

	if (pkg_name_len) {
		++mod_name;  /* skip '.' */
	}

	mod_foreach(mod) {
		if (strcmp(mod->name, mod_name)) {
			continue;
		}
		if (strncmp(mod->package->name, fqn, pkg_name_len) ||
		    mod->package->name[pkg_name_len]) {
			continue;
		}
		return mod;
	}

	return NULL;
}

