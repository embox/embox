/**
 * @file
 * @brief EMBuild-side binding interface.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_
#define FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_

#define USE_MOD_NAMES \
	OPTION_MODULE_GET(embox__framework__embuild_light,BOOLEAN,use_mod_names)

#if 1 /* switch some mod_def to tottaly empty will help estimate how much
		 mod_def is taking space */
#define __MOD_DEF(mod_nm) \
	ARRAY_SPREAD_DEF_TERMINATED(const struct mod_member *, \
			__MOD_MEMBERS(mod_nm), NULL); /* FIXME Potentially isn't necessary */ \
	extern const struct mod_app __MOD_APP(mod_nm) \
			__attribute__ ((weak)); \
	extern const struct mod_build_info __MOD_BUILDINFO(mod_nm) \
			__attribute__((weak)); \
	\
	extern struct logger __MOD_LOGGER(_mod_nm)         \
			__attribute__ ((weak));                          \
	extern const struct mod __MOD(mod_nm) __attribute__((weak)); \
	\
	ARRAY_SPREAD_DECLARE(const struct mod *const,      \
			__mod_registry);                      \
	ARRAY_SPREAD_ADD(__mod_registry, &__MOD(mod_nm)) /* TODO don't like it. -- Eldar */
#else
#define __MOD_DEF(mod_nm)
#endif

#define __MOD_SEC_LABEL_DEF(mod_nm)
#define __MOD_LABEL_DEF(mod_nm)

#define __MOD_LOGGER_DEF(mod_nm, log_level)

#define __MOD_APP_DEF(mod_nm)
#define __MOD_CMD_DEF(mod_nm, cmd_name, cmd_brief, cmd_details) \
	const struct cmd_desc __MOD_CMD(mod_nm) = { \
		.name    = cmd_name, \
	}

#if USE_MOD_NAMES
#define __MOD_BUILDINFO_DEF(_mod_nm, _package_name, _mod_name) \
	const struct mod_build_info __MOD_BUILDINFO(_mod_nm) = { \
		.pkg_name   = _package_name,                         \
		.mod_name   = _mod_name,                             \
	}
#else
#define __MOD_BUILDINFO_DEF(_mod_nm, _package_name, _mod_name)
#endif

#define __MOD_DEP_DEF(mod_nm, dep_nm)
#define __MOD_CONTENTS_DEF(mod_nm, content_nm)
#define __MOD_AFTER_DEP_DEF(mod_nm, dep_nm)

#endif /* FRAMEWORK_MOD_EMBUILD_LIGHT_IMPL_H_ */
