/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_MOD_DECLS_H_
#define IMPL_MOD_DECLS_H_

/* Internal variable names inside mods-private namespace. */

#define __MOD(s_mod)              __mod__$$##s_mod
#define __MOD_INFO(s_mod)         __mod_info__$$##s_mod
#define __MOD_NAME(s_mod)         __mod_name__$$##s_mod
#define __MOD_PRIVATE(s_mod)      __mod_private__$$##s_mod
#define __MOD_PACKAGE(s_package)  __mod_package__$$##s_package

/* Internal declarations. */

#define __MOD_DECL(s_mod) \
	extern const struct mod __MOD(s_mod)

#define __MOD_PACKAGE_DECL(s_package) \
	extern const struct mod_package __MOD_PACKAGE(s_package)

#define __MOD_INFO_DECL(s_mod) \
	extern const struct mod_info __MOD_INFO(s_mod) __attribute__ ((weak))

#endif /* IMPL_MOD_DECLS_H_ */
