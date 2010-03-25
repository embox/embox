/**
 * @file
 * @brief TODO
 *
 * @date 09.03.2010
 * @author Eldar Abusalimov
 */

#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <embox/kernel.h>

#include <embox/mod.h>

#define mod_flag_tst(mod, mask)   ((mod)->private->flags &   (mask))
#define mod_flag_tgl(mod, mask) do (mod)->private->flags ^=  (mask); while(0)
#define mod_flag_set(mod, mask) do (mod)->private->flags |=  (mask); while(0)
#define mod_flag_clr(mod, mask) do (mod)->private->flags &= ~(mask); while(0)

static int mod_perform(const struct mod *mod, bool op);
static int mod_perform_nodep(const struct mod *mod, bool op);
static bool mod_deps_satisfied(const struct mod *mod, bool op);
/* Define generic package. */
MOD_PACKAGE_DEF(generic, "generic");

inline static mod_op_t mod_op_deref(const struct mod *mod, size_t op_offset) {
	if (NULL != mod->ops_ref && NULL != mod->ops_ref->ops) {
		return *(mod_op_t *) ((size_t) mod->ops_ref->ops + op_offset);
	}
	return NULL;
}

inline static void *mod_data_deref(const struct mod *mod) {
	return (NULL != mod->data_ref) ? mod->data_ref->data : NULL;
}

void *mod_data(const struct mod *mod) {
	if (NULL == mod) {
		return NULL;
	}
	return mod_data_deref(mod);
}

int mod_invoke(const struct mod *mod, void *data) {
	mod_invoke_t invoke;

	if (NULL == mod) {
		return -EINVAL;
	}
	if (NULL == mod->ops_ref || NULL == mod->ops_ref->ops || NULL == (invoke
			= mod->ops_ref->ops->invoke)) {
		return -ENOTSUP;
	}
	return invoke((struct mod *) mod, data);
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
	struct mod **p_dep;

	if (!!op == !!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return true;
	}

	for (p_dep = op ? mod->requires : mod->provides; *p_dep != NULL; ++p_dep) {
		if (!!op != !!mod_flag_tst(*p_dep, MOD_FLAG_ENABLED)) {
			return false;
		}
	}

	return true;
}

static int mod_perform(const struct mod *mod, bool op) {
	struct mod **p_dep;

	if (!!op == !!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	for (p_dep = op ? mod->requires : mod->provides; *p_dep != NULL; ++p_dep) {
		if (0 != mod_perform(*p_dep, op)) {
			return -EINTR;
		}
	}

	return mod_perform_nodep(mod, op);
}

// XXX What's about recursive invocations from mod ops? -- Eldar
// TODO introduce -ELOOP or something else.
static int mod_perform_nodep(const struct mod *mod, bool op) {
	size_t op_offset = op ? offsetof(struct mod_ops, enable)
			: offsetof(struct mod_ops, disable);
	mod_op_t mod_op;

	if (!!op == !!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	if (NULL != (mod_op = mod_op_deref(mod, op_offset)) && 0 != mod_op(
			(struct mod *) mod)) {
		mod_flag_set(mod, MOD_FLAG_OPFAILED);
		return -EINTR;
	}

	mod_flag_clr(mod, MOD_FLAG_OPFAILED);
	mod_flag_tgl(mod, MOD_FLAG_ENABLED);
	return 0;
}
