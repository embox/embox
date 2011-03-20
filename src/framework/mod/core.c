/**
 * @file
 * @brief Core dependency resolution.
 *
 * @date 09.03.2010
 * @author Eldar Abusalimov
 */

#include <stddef.h>
#include <stdbool.h>
#include <errno.h>

#include <util/array.h>

#include <framework/mod/api.h>
#include "types.h"

#define MOD_FLAG_ENABLED       __MOD_FLAG_ENABLED

// TODO unused for now... -- Eldar
#define MOD_FLAG_OPFAILED      (0 << 1)
#define MOD_FLAG_OPINPROGRESS  (0 << 2)

#define mod_flag_tst(mod, mask) __mod_flag_tst(mod, mask)
#define mod_flag_tgl(mod, mask) do (mod)->private->flags ^=  (mask); while (0)
#define mod_flag_set(mod, mask) do (mod)->private->flags |=  (mask); while (0)
#define mod_flag_clr(mod, mask) do (mod)->private->flags &= ~(mask); while (0)

ARRAY_SPREAD_DEF_TERMINATED(const struct mod *, __mod_registry, NULL);

static int mod_perform(const struct mod *mod, bool op);
static int mod_perform_nodep(const struct mod *mod, bool op);
static bool mod_deps_satisfied(const struct mod *mod, bool op);

inline static mod_op_t mod_op_deref(const struct mod *mod, bool op) {
	struct mod_ops *ops;
	if (NULL == mod->info || NULL == (ops = mod->info->ops)) {
		return NULL;
	}
	return op ? ops->enable : ops->disable;
}

int mod_enable(const struct mod *mod) {
	if (NULL == mod) {
		return -EINVAL;
	}
	return mod_perform(mod, true);
}

int mod_disable(const struct mod *mod) {
	if (NULL == mod) {
		return -EINVAL;
	}
	return mod_perform(mod, false);
}

int mod_enable_nodep(const struct mod *mod) {
	if (NULL == mod) {
		return -EINVAL;
	}
	if (!mod_deps_satisfied(mod, true)) {
		return -EBUSY;
	}
	return mod_perform_nodep(mod, true);
}

int mod_disable_nodep(const struct mod *mod) {
	if (NULL == mod) {
		return -EINVAL;
	}
	if (!mod_deps_satisfied(mod, false)) {
		return -EBUSY;
	}
	return mod_perform_nodep(mod, false);
}

static bool mod_deps_satisfied(const struct mod *mod, bool op) {
	const struct mod *dep;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return true;
	}

	array_nullterm_foreach(dep, op ? mod->requires : mod->provides) {
		if (!op != !mod_flag_tst(dep, MOD_FLAG_ENABLED)) {
			return false;
		}
	}

	return true;
}

static int mod_perform(const struct mod *mod, bool op) {
	const struct mod *dep;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	array_nullterm_foreach(dep, op ? mod->requires : mod->provides) {
		if (0 != mod_perform(dep, op)) {
			return -EINTR;
		}
	}

	return mod_perform_nodep(mod, op);
}

// XXX What's about recursive invocations from mod ops? -- Eldar
// TODO introduce -ELOOP or something else.
static int mod_perform_nodep(const struct mod *mod, bool op) {
	mod_op_t mod_op;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	mod_op = mod_op_deref(mod, op);
	if (NULL != mod_op && 0 != mod_op((struct mod *) mod)) {
		mod_flag_set(mod, MOD_FLAG_OPFAILED);
		return -EINTR;
	}

	mod_flag_clr(mod, MOD_FLAG_OPFAILED);
	mod_flag_tgl(mod, MOD_FLAG_ENABLED);
	return 0;
}
