/**
 * @file
 * @brief TODO
 *
 * @date 09.03.2010
 * @author Eldar Abusalimov
 */

#include <stddef.h>
#include <errno.h>
#include <common.h>

#include <embox/mod.h>

#define mod_flag_tst(mod, mask)   ((mod)->private->flags &   (mask))
#define mod_flag_set(mod, mask) do (mod)->private->flags |=  (mask); while(0)
#define mod_flag_clr(mod, mask) do (mod)->private->flags &= ~(mask); while(0)

MOD_PACKAGE_DEF(generic, "generic");

int mod_enable(const struct mod *mod) {
	struct mod **p_requires;

	if (NULL == mod) {
		return -EINVAL;
	}
	if (mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	for (p_requires = mod->requires; *p_requires != NULL; ++p_requires) {
		if (0 != mod_enable(*p_requires)) {
			return -EINTR;
		}
	}

#if 0
	TRACE("mod: enabling mod %s.%s\n", mod->package->name, mod->name);
	if (NULL != mod->ops) {
		TRACE("     mod->ops: 0x%08x\n", (unsigned int) mod->ops);
		TRACE("     mod->ops->enable: 0x%08x\n", (unsigned int) (*mod->ops)->enable);
	}
#endif
	if (NULL != mod->ops_ref && NULL != mod->ops_ref->ops->enable && 0
			!= mod->ops_ref->ops->enable((struct mod *) mod)) {
		mod_flag_set(mod, MOD_FLAG_OPFAILED);
		return -EINTR;
	}

	mod_flag_clr(mod, MOD_FLAG_OPFAILED);
	mod_flag_set(mod, MOD_FLAG_ENABLED);
	return 0;
}

int mod_disable(const struct mod *mod) {
	struct mod **p_provides;

	if (NULL == mod) {
		return -EINVAL;
	}
	if (!mod_flag_tst(mod, MOD_FLAG_ENABLED)) {
		return 0;
	}

	for (p_provides = mod->provides; *p_provides != NULL; ++p_provides) {
		if (0 != mod_disable(*p_provides)) {
			return -EINTR;
		}
	}

	if (NULL != mod->ops_ref && NULL != mod->ops_ref->ops->disable && 0
			!= mod->ops_ref->ops->disable((struct mod *) mod)) {
		mod_flag_set(mod, MOD_FLAG_OPFAILED);
		return -EINTR;
	}

	mod_flag_clr(mod, MOD_FLAG_OPFAILED);
	mod_flag_clr(mod, MOD_FLAG_ENABLED);
	return 0;
}
