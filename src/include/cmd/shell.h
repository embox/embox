/**
 * @file
 * @brief
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#ifndef CMD_SHELL_H_
#define CMD_SHELL_H_

#include <util/array.h>
#include <framework/mod/self.h>

extern int shell_line_input(const char *line);



typedef void (*shell_run_ft)(void);

struct shell {
	const char * name;
	shell_run_ft exec;
};

extern const struct shell *shell_lookup(const char *shell_name);

#define SHELL_DEF(_exec,sh_name) \
	extern const struct shell __shell_registry[]; \
	static void _exec(void);  \
	ARRAY_SPREAD_ADD(__shell_registry, {        \
			.exec = _exec,            \
			.name = "" sh_name        \
		})

#ifdef __CDT_PARSER__

# undef  SHELL_DEF
# define SHELL_DEF(_exec) \
	static void _exec(void); \


#endif /* __CDT_PARSER__ */



#endif
