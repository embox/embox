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

#include <util/array.h>

#include <framework/mod/api.h>
#include "types.h"

#define MOD_FLAG_ENABLED       (1 << 0)

// TODO unused for now... -- Eldar
#define MOD_FLAG_OPFAILED      (0 << 1)
#define MOD_FLAG_OPINPROGRESS  (0 << 2)

#define mod_flag_tst(mod, mask)   ((mod)->private->flags &   (mask))
#define mod_flag_tgl(mod, mask) do (mod)->private->flags ^=  (mask); while (0)
#define mod_flag_set(mod, mask) do (mod)->private->flags |=  (mask); while (0)
#define mod_flag_clr(mod, mask) do (mod)->private->flags &= ~(mask); while (0)

ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __mod_registry, NULL);

static int mod_traverse(const struct mod *mod, bool op);
static bool mod_traverse_needed(const struct mod *mod, bool op);
static int do_enable(const struct mod *mod);
static int do_disable(const struct mod *mod);

bool mod_is_running(const struct mod *mod) {
	return mod && mod_flag_tst(mod, MOD_FLAG_ENABLED);
}

int mod_enable(const struct mod *mod) {
	if (!mod) {
		return -EINVAL;
	}
	return mod_traverse(mod, true);
}

int mod_disable(const struct mod *mod) {
	if (!mod) {
		return -EINVAL;
	}
	return mod_traverse(mod, false);
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

// XXX What's about recursive invocations from mod ops? -- Eldar
// TODO introduce -ELOOP or something else.
static int mod_traverse(const struct mod *mod, bool op) {
	const struct mod *dep;
	const struct mod **deps = op ? mod->requires : mod->provides;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	array_nullterm_foreach(dep, deps) {
		if (0 != mod_traverse(dep, op)) {
			return -EINTR;
		}
	}

	return op ? do_enable(mod) : do_disable(mod);
}

static const struct mod_ops *mod_ops_deref(const struct mod *mod) {
	const struct mod_info *info = mod->info;
	return info ? info->ops : NULL;
}

static int invoke_mod_enable(const struct mod *mod) {
	const struct mod_ops *ops = mod_ops_deref(mod);
	return ops ? ops->enable((struct mod *) mod) : 0;
}

static int invoke_mod_disable(const struct mod *mod) {
	const struct mod_ops *ops = mod_ops_deref(mod);
	return ops ? ops->disable((struct mod *) mod) : 0;
}

static int invoke_member_init(const struct mod_member_info *member) {
	const struct mod_member_ops *ops = member->ops;
	return ops ? ops->init((struct mod_member_info *) member) : 0;
}

static int invoke_member_fini(const struct mod_member_info *member) {
	const struct mod_member_ops *ops = member->ops;
	return ops ? ops->fini((struct mod_member_info *) member) : 0;
}

static int do_enable(const struct mod *mod) {
	const struct mod_member_info *member;

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
	const struct mod_member_info *member;

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

