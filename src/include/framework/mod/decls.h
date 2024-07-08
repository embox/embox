/**
 * @file
 * @brief Mod-private namespace declarations.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 * @author Anton Kozlov
 *		- options support
 */

#ifndef FRAMEWORK_MOD_DECLS_H_
#define FRAMEWORK_MOD_DECLS_H_

#include <util/macro.h>

/*
 * Implementation note:
 * Many macros use some of their arguments to construct symbol names. This
 * imposes well-known restrictions to the input values of these arguments.
 * To prevent a confusion all such arguments are suffixed by '_nm' (name).
 */

/* Internal variable names inside mods-private namespace. */

#define __MOD(mod_nm)            MACRO_CONCAT(__mod__,            mod_nm)
#define __MOD_INFO(mod_nm)       MACRO_CONCAT(__mod_info__,       mod_nm)

#define __MOD_LABEL(mod_nm)      MACRO_CONCAT(__mod_label__,      mod_nm)
#define __MOD_SEC_LABEL(mod_nm)  MACRO_CONCAT(__mod_sec_label__,  mod_nm)
#define __MOD_LOGGER(mod_nm)     MACRO_CONCAT(__mod_logger__,     mod_nm)
#define __MOD_CMD(mod_nm)        MACRO_CONCAT(__mod_cmd__,        mod_nm)
#define __MOD_APP(mod_nm)        MACRO_CONCAT(__mod_app__,        mod_nm)
#define __MOD_PRIVATE(mod_nm)    MACRO_CONCAT(__mod_private__,    mod_nm)
#define __MOD_PROVIDES(mod_nm)   MACRO_CONCAT(__mod_provides__,   mod_nm)
#define __MOD_DEPENDS(mod_nm)    MACRO_CONCAT(__mod_depends__,    mod_nm)
#define __MOD_AFTER_DEPS(mod_nm) MACRO_CONCAT(__mod_after_deps__, mod_nm)
#define __MOD_CONTENTS(mod_nm)   MACRO_CONCAT(__mod_contents__,   mod_nm)
#define __MOD_MEMBERS(mod_nm)    MACRO_CONCAT(__mod_members__,    mod_nm)

#define __MOD_PACKAGE(pkg_nm)    MACRO_CONCAT(__mod_package__,    pkg_nm)

#define __MOD_BUILDINFO(mod_nm)  MACRO_CONCAT(__mod_buildinfo__,  mod_nm)

#define __RUNLEVEL_LAST_MODULE(nr) MACRO_CONCAT(__runlevel_last_module, nr)

#endif /* FRAMEWORK_MOD_DECLS_H_ */
