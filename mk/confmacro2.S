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
	mod | __FIRST_ARG(__VA_ARGS__) |

#define test(mod_name, ...) \
	embox.test.##mod_name | __FIRST_ARG(__VA_ARGS__) |

#define example(mod_name, ...) \
	embox.example.##mod_name | __FIRST_ARG(__VA_ARGS__) |

#define cmd(mod_name) \
	embox.cmd.##mod_name

#include "mods.conf"

#else
#error "Illegal invocation"
#endif
