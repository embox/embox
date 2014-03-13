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

ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __mod_registry, NULL);
ARRAY_SPREAD_DEF_TERMINATED(const struct mod_sec_label *, __mod_sec_labels, NULL);

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
	const struct mod_app *app = mod->app;

	if (app)
		/* No need to traverse deps here, they are already initialized. */
		memcpy(app->data + APP_DATA_RESERVE_OFFSET, app->data, app->data_sz);
}

int mod_activate_app(const struct mod *mod) {
	const struct mod_app *app;

	if (!mod_is_running(mod))
		return -ENOENT;

	app = mod->app;
	if (app) {
		const struct mod *dep;

		array_spread_nullterm_foreach(dep, mod->requires) {
			int ret = mod_activate_app(dep);
			if (ret)
				return ret;
		}

		memcpy(app->data, app->data + APP_DATA_RESERVE_OFFSET, app->data_sz);
		memset(app->bss, 0, app->bss_sz);
	}

	return 0;
}

bool mod_check(const struct mod *mod) {
	const struct mod_sec_label *sec_label;
	assert(mod);

	if (!mod->label)
		return true;

	array_spread_nullterm_foreach(sec_label, __mod_sec_labels) {
		if (sec_label->mod == mod)
			return !memcmp(&sec_label->label, mod->label, sizeof(*mod->label));
	}

	return true;
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

int mod_enable(const struct mod *mod) {
	const struct mod_member *member;
	const struct mod_info *info;

	if (mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	mod_init_app(mod);

	array_spread_nullterm_foreach(member, mod->members) {
		if (0 != invoke_member_init(member)) {
			goto opfailed;
		}
	}

	info = mod->info;
	if (info && info->ops && info->ops->enable) {
		info->ops->enable(info);
	}

	mod_flag_tgl(mod, MOD_FLAG_ENABLED);

	return 0;
opfailed:
	mod_flag_set(mod, MOD_FLAG_OPFAILED);
	return -EINTR;
}

int mod_disable(const struct mod *mod) {
	const struct mod_member *member;
	const struct mod_info *info;

	if (!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	info = mod->info;
	if (info && info->ops && info->ops->disable) {
		info->ops->disable(info);
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
