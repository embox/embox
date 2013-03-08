/**
 * @file
 * @brief Core dependency resolution.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#include <stddef.h>
#include <stdbool.h>
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

#define mod_flag_tst(mod, mask)   ((mod)->private->flags &   (mask))
#define mod_flag_tgl(mod, mask) do (mod)->private->flags ^=  (mask); while (0)
#define mod_flag_set(mod, mask) do (mod)->private->flags |=  (mask); while (0)
#define mod_flag_clr(mod, mask) do (mod)->private->flags &= ~(mask); while (0)

ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __mod_registry, NULL);

static int mod_traverse(const struct mod *mod, bool op);
static int mod_traverse_rec_safe(const struct mod *mod, bool op, bool recursive_safe);
static bool mod_traverse_needed(const struct mod *mod, bool op);
static int do_enable(const struct mod *mod);
static int do_disable(const struct mod *mod);

bool mod_is_running(const struct mod *mod) {
	return mod && mod_flag_tst(mod, MOD_FLAG_ENABLED);
}

int mod_enable_rec_safe(const struct mod *mod, bool recursive_safe) {
	if (!mod) {
		return -EINVAL;
	}
	return mod_traverse_rec_safe(mod, true, recursive_safe);
}


int mod_enable(const struct mod *mod) {
	return mod_enable_rec_safe(mod, false);
}

int mod_disable_rec_safe(const struct mod *mod, bool recursive_safe) {
	if (!mod) {
		return -EINVAL;
	}
	return mod_traverse_rec_safe(mod, false, recursive_safe);
}

int mod_disable(const struct mod *mod) {
	return mod_disable_rec_safe(mod, false);
}

int mod_enable_nodep(const struct mod *mod) {
	if (!mod) {
		return -EINVAL;
	}
	if (mod_traverse_needed(mod, true)) {
		return -EBUSY;
	}
	return do_enable(mod);
}

int mod_disable_nodep(const struct mod *mod) {
	if (!mod) {
		return -EINVAL;
	}
	if (mod_traverse_needed(mod, false)) {
		return -EBUSY;
	}
	return do_disable(mod);
}

static bool mod_traverse_needed(const struct mod *mod, bool op) {
	const struct mod *dep;
	const struct mod **deps = op ? mod->requires : mod->provides;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return false;
	}

	array_nullterm_foreach(dep, deps) {
		if (!op != !mod_flag_tst(dep, MOD_FLAG_ENABLED)) {
			return true;
		}
	}

	return false;
}

static int mod_traverse_do(const struct mod **mods, bool op, bool soft) {
	const struct mod *mod;

	array_nullterm_foreach(mod, mods) {
		if (soft && mod_flag_tst(mod, MOD_FLAG_OPINPROGRESS)) {
			continue;
		}

		if (0 != mod_traverse(mod, op)) {
			return -EINTR;
		}
	}

	return 0;
}

static int mod_traverse_rec_safe(const struct mod *mod, bool op, bool recursive_safe) {
	const struct mod **deps = op ? mod->requires : mod->provides;
	const struct mod **after_deps = mod->after_deps;
	int ret;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	if (!recursive_safe) {
		assert(0 == mod_flag_tst(mod, MOD_FLAG_OPINPROGRESS),
				"Recursive mod traversing");
	} else {
		if (mod_flag_tst(mod, MOD_FLAG_OPINPROGRESS)) {
			return 0;
		}
	}

	mod_flag_tgl(mod, MOD_FLAG_OPINPROGRESS);

	if (!op && (ret = mod_traverse_do(after_deps, op, 1))) {
		return ret;
	}

	if ((ret = mod_traverse_do(deps, op, 0))) {
		return ret;
	}

	if ((ret = (op ? do_enable(mod) : do_disable(mod)))) {
		return ret;
	}

	mod_flag_tgl(mod, MOD_FLAG_OPINPROGRESS);

	if ((ret = mod_traverse_do(after_deps, op, 1))) {
		return ret;
	}

	return 0;
}

static int mod_traverse(const struct mod *mod, bool op) {
	return mod_traverse_rec_safe(mod, op, false);
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

static int do_enable(const struct mod *mod) {
	const struct mod_member *member;

	if (mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

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


static int do_disable(const struct mod *mod) {
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
