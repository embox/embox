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

#define MOD_FLAG_ENABLED       (1 << 0)

// TODO unused for now... -- Eldar
#define MOD_FLAG_OPFAILED      (0 << 1)
#define MOD_FLAG_OPINPROGRESS  (0 << 2)

#define mod_flag_tst(mod, mask)   ((mod)->private->flags &   (mask))
#define mod_flag_tgl(mod, mask) do (mod)->private->flags ^=  (mask); while(0)
#define mod_flag_set(mod, mask) do (mod)->private->flags |=  (mask); while(0)
#define mod_flag_clr(mod, mask) do (mod)->private->flags &= ~(mask); while(0)

static int mod_perform(const struct mod *mod, bool op);
static int mod_perform_nodep(const struct mod *mod, bool op);
static bool mod_deps_satisfied(const struct mod *mod, bool op);
/* Define generic package. */
MOD_PACKAGE_DEF(generic, "generic");

inline static mod_op_t mod_op_deref(const struct mod *mod, bool op) {
	if (NULL != mod->api && NULL != mod->api->ops) {
		return op ? mod->api->ops->enable : mod->api->ops->disable;
	}
	return NULL;
}

inline static void *mod_data_deref(const struct mod *mod) {
	return (NULL != mod->api) ? mod->api->data : NULL;
}

struct mod_iterator *mod_requires(const struct mod *mod,
		struct mod_iterator *iterator) {
	if (NULL == mod || NULL == iterator) {
		return NULL;
	}
	iterator->p_mod = mod->requires;
	return iterator;
}

struct mod_iterator *mod_provides(const struct mod *mod,
		struct mod_iterator *iterator) {
	if (NULL == mod || NULL == iterator) {
		return NULL;
	}
	iterator->p_mod = mod->provides;
	return iterator;
}

struct mod_iterator *mod_tagged(const struct mod_tag *tag,
		struct mod_iterator *iterator) {
	if (NULL == tag || NULL == iterator) {
		return NULL;
	}
	iterator->p_mod = tag->mods;
	return iterator;
}


inline bool mod_iterator_has_next(struct mod_iterator *iterator) {
	return NULL != iterator && NULL != iterator->p_mod && NULL
			!= *iterator->p_mod;
}

struct mod *mod_iterator_next(struct mod_iterator *iterator) {
	if (!mod_iterator_has_next(iterator)) {
		return NULL;
	}
	return *(iterator->p_mod++);
}

bool mod_is_running(const struct mod *mod) {
	return mod != NULL && mod_flag_tst(mod, MOD_FLAG_ENABLED);
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
	if (NULL == mod->api || NULL == mod->api->ops ||
			NULL == (invoke = mod->api->ops->invoke)) {
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

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return true;
	}

	for (p_dep = op ? mod->requires : mod->provides; *p_dep != NULL; ++p_dep) {
		if (!op != !mod_flag_tst(*p_dep, MOD_FLAG_ENABLED)) {
			return false;
		}
	}

	return true;
}

static int mod_perform(const struct mod *mod, bool op) {
	struct mod **p_dep;

	if (!op == !mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
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
