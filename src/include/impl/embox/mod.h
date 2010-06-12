/**
 * @file
 * @brief TODO
 *
 * @date 10.06.2010
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_MOD_H_
# error "Do not include this file directly, use <embox/mod.h> instead!"
#endif /* EMBOX_MOD_H_ */

#include <util/macro.h>

/*
 * Implementation note:
 * Many macros uses some of their arguments to construct symbol names. This
 * imposes well-known restrictions to the input values of these arguments.
 * To prevent a confusion all such arguments are prefixed by 's_' (symbol).
 */

#if !defined(__FRAMEWORK__) && !defined(__EMBUILD_DEPSINJECT__)
# ifndef __EMBUILD_MOD__
#  error "Do not include without __EMBUILD_MOD__ defined (e.g. from lib code)!"
# endif /* __EMBUILD_MOD__ */
#endif /* __EMBUILD_DEPSINJECT__ */

/* Linker sections stuff. */

/*
 * All mod-related sections are linked using something like
 * *(SORT(.mod*.rodata)). SORT guarantees that the wildcard is expanded in the
 * lexicographical order (order argument is used to control the placement of
 * sections within a single mod). We use this facility to create
 * null-terminated arrays of module dependencies initialized (populated) in
 * multiple compilation units.
 */

#define __MOD_SECTION(s_mod, section, ord, tag) \
	".mod" MACRO_STRING(__##section##__$$##s_mod##$$__##ord##_##tag) ".rodata"

#define __MOD_SECTION_HEAD(s_mod, section) __MOD_SECTION(s_mod, section,0,head)
#define __MOD_SECTION_BODY(s_mod, section) __MOD_SECTION(s_mod, section,1,body)
#define __MOD_SECTION_TAIL(s_mod, section) __MOD_SECTION(s_mod, section,9,tail)

/* Internal variable names. */

#define __MOD(s_mod)              __mod__$$##s_mod
#define __MOD_INFO(s_mod)         __mod_info__$$##s_mod
#define __MOD_TAG(s_tag)          __mod_tag__$$##s_tag
#define __MOD_NAME(s_mod)         __mod_name__$$##s_mod
#define __MOD_PRIVATE(s_mod)      __mod_private__$$##s_mod
#define __MOD_PACKAGE(s_package)  __mod_package__$$##s_package
#define __MOD_ARRAY(s_mod, s_array) __mod_##s_array##__$$##s_mod
#define __MOD_ARRAY_ENTRY(s_mod, s_array, s_entry) \
	__mod_##s_array##__$$##s_mod##$$__$$##s_entry

/* Internal declarations and definitions. */

#define __MOD_DECL(s_mod) \
	extern const struct mod __MOD(s_mod)

#define __MOD_PACKAGE_DECL(s_package) \
	extern const struct mod_package __MOD_PACKAGE(s_package)

#define __MOD_INFO_DECL(s_mod) \
	extern const struct mod_info __MOD_INFO(s_mod) __attribute__ ((weak))

#define __MOD_TAG_DECL(s_tag) \
	extern const struct mod_tag __MOD_TAG(s_tag)

#define __MOD_NAME_DEF(s_mod, mod_name) \
	const char __MOD_NAME(s_mod)[] = mod_name
#define __MOD_PRIVATE_DEF(s_mod) \
	static struct mod_private __MOD_PRIVATE(s_mod)

#define __MOD_ARRAY_DEF(s_mod, s_array) \
	__extension__ static const struct mod *__MOD_ARRAY(s_mod, s_array)[0] \
		__attribute__ ((section(__MOD_SECTION_HEAD(s_mod, s_array)))); \
	static const struct mod *__MOD_ARRAY_ENTRY(s_mod, s_array, __null$$) \
		__attribute__ ((used, section(__MOD_SECTION_TAIL(s_mod, s_array)))) \
		= NULL

#define __MOD_ARRAY_ADD(s_mod, s_array, s_mod_entry) \
	static const struct mod *__MOD_ARRAY_ENTRY(s_mod, s_array, s_mod_entry) \
		__attribute__ ((used, section(__MOD_SECTION_BODY(s_mod, s_array)))) \
		= MOD_PTR(s_mod_entry) \

/* Here goes public macros API implementation. */

#define __MOD_PTR(s_mod) (&__MOD(s_mod))

#define __MOD_TAG_PTR(s_tag) (&__MOD_TAG(s_tag))

#define __MOD_DEF(s_mod, s_mod_pkg, mod_name) \
		__MOD_INFO_DECL(s_mod); \
		__MOD_PACKAGE_DECL(s_mod_pkg); \
		__MOD_ARRAY_DEF(s_mod, requires); \
		__MOD_ARRAY_DEF(s_mod, provides); \
		__MOD_NAME_DEF(s_mod, mod_name); \
		__MOD_PRIVATE_DEF(s_mod); \
		const struct mod __MOD(s_mod) = { \
				.private  = &__MOD_PRIVATE(s_mod), \
				.info      = (struct mod_info *) &__MOD_INFO(s_mod), \
				.package  = (struct mod_package *) &__MOD_PACKAGE(s_mod_pkg), \
				.name     = __MOD_NAME(s_mod), \
				.requires = (struct mod **) &__MOD_ARRAY(s_mod, requires), \
				.provides = (struct mod **) &__MOD_ARRAY(s_mod, provides), \
			}

#define __MOD_TAG_DEF(s_tag, tag_name) \
		__MOD_ARRAY_DEF(s_tag, tagged); \
		const struct mod_tag __MOD_TAG(s_tag) = { \
				.name = tag_name, \
				.mods = (struct mod **) &__MOD_ARRAY(s_tag, tagged), \
			}

#define __MOD_INFO_DEF(s_mod, mod_data, mod_ops) \
		__MOD_INFO_DEF__(s_mod, mod_data, mod_ops, NULL)

#define __MOD_INFO_TAGGED_DEF(s_mod, mod_data, mod_ops, s_tag) \
		__MOD_DECL(s_mod); \
		__MOD_TAG_DECL(s_tag); \
		__MOD_ARRAY_ADD(s_tag, tagged, s_mod); \
		__MOD_INFO_DEF__(s_mod, mod_data, mod_ops, MOD_TAG_PTR(s_tag))

#define __MOD_INFO_DEF__(s_mod, _mod_data, _mod_ops, _mod_tag) \
		const struct mod_info __MOD_INFO(s_mod) = { \
				.data = (void *) _mod_data, \
				.ops = (struct mod_ops *) _mod_ops, \
				.tag = (struct mod_tag *) _mod_tag, \
			}

#define __MOD_DEP_DEF(s_mod, s_dep) \
		__MOD_DECL(s_mod); \
		__MOD_DECL(s_dep); \
		__MOD_ARRAY_ADD(s_mod, requires, s_dep); \
		__MOD_ARRAY_ADD(s_dep, provides, s_mod)

#define __MOD_PACKAGE_DEF(s_package, package_name) \
		const struct mod_package __MOD_PACKAGE(s_package) = { \
				.name = package_name, \
			}

// TODO this should be removed at all. -- Eldar
#define MOD_ROOT_DEF(s_mod) \
	static const struct mod *__mod_root__##s_mod \
		__attribute__ ((used, section(".mod.rodata"))) = MOD_PTR(s_mod)

/*
 * Macros with 'SELF' suffix are used to access the self module of the current
 * compilation unit (referenced by __EMBUILD_MOD__), e.g. to bind something
 * like #mod_data.
 */

#define __MOD_SELF(macro) \
		__MOD_SELF_EXPAND(macro, __EMBUILD_MOD__)
#define __MOD_SELF_EXPAND(macro, mod) \
		__MOD_SELF_INVOKE(macro, mod)
#define __MOD_SELF_INVOKE(macro, mod) \
		macro(mod)

#define __MOD_SELF_PTR  __MOD_SELF(MOD_PTR)

#define __MOD_SELF_NAME __MOD_SELF(__MOD_NAME)

#define __MOD_SELF_INFO_DEF(_mod_data, _mod_ops) \
		__MOD_INFO_DEF(__EMBUILD_MOD__, _mod_data, _mod_ops)

#define __MOD_SELF_INFO_TAGGED_DEF(_mod_data, _mod_ops, s_tag) \
		__MOD_INFO_TAGGED_DEF(__EMBUILD_MOD__, _mod_data, _mod_ops, s_tag)

#define __MOD_SELF_DEP_DEF(s_dep) MOD_DEP_DEF(__EMBUILD_MOD__, s_dep)

