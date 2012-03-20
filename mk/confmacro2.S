/*
 * Embox C PreProcessor-based Configuration file
 *
 * Author: Eldar Abusalimov
 */

#define __FIRST_ARG(arg, ...) arg

#ifdef __MODS_CONF__
#define option(option, ...) \
	$(warning Options defined outside "options.conf" are ignored)

#define mod(mod, ...) \
	\tinclude mod

#define test(mod_name, ...) \
	\tinclude embox.test.##mod_name

#define example(mod_name, ...) \
	\tinclude embox.example.##mod_name

#define cmd(mod_name) \
	\tinclude embox.cmd.##mod_name

package genConfig

configuration genCfg {

#include "mods.conf"

}

#else
#error "Illegal invocation"
#endif
