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

#include <module/embox/framework/mod.h>

/**
 * Implementation note:
 * Many macros use some of their arguments to construct symbol names. This
 * imposes well-known restrictions to the input values of these arguments.
 * To prevent a confusion all such arguments are suffixed by '_nm' (name).
 */

/* Internal variable names inside mods-private namespace */
#define __MOD(mod_nm)          MACRO_CONCAT(__mod__, mod_nm)
#define __MOD_APP(mod_nm)      MACRO_CONCAT(__mod_app__, mod_nm)
#define __MOD_CMD(mod_nm)      MACRO_CONCAT(__mod_cmd__, mod_nm)
#define __MOD_DATA(mod_nm)     MACRO_CONCAT(__mod_data__, mod_nm)
#define __MOD_LABEL(mod_nm)    MACRO_CONCAT(__mod_label__, mod_nm)
#define __MOD_LOGGER(mod_nm)   MACRO_CONCAT(__mod_logger__, mod_nm)
#define __MOD_DEPENDS(mod_nm)  MACRO_CONCAT(__mod_depends__, mod_nm)
#define __MOD_PROVIDES(mod_nm) MACRO_CONCAT(__mod_provides__, mod_nm)

/* Numeric/string values ​​contained in <module/embox/framework/mod.h> */
#define __MOD_NAME(mod_nm)     MACRO_CONCAT(__mod_name__, mod_nm)
#define __MOD_PACKAGE(mod_nm)  MACRO_CONCAT(__mod_package__, mod_nm)
#define __MOD_RUNLEVEL(mod_nm) MACRO_CONCAT(__mod_runlevel__, mod_nm)
#define __MOD_PRIORITY(mod_nm) MACRO_CONCAT(__mod_priority__, mod_nm)

#endif /* FRAMEWORK_MOD_DECLS_H_ */
