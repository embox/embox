/**
 * @file
 * @brief
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#ifndef CMD_SHELL_H_
#define CMD_SHELL_H_

#include <stddef.h>
#include <errno.h>
#include <util/log.h>
#include <lib/libds/array.h>
#include <framework/mod/self.h>

#include <framework/mod/options.h>
#include <module/embox/cmd/sh/shell_registry.h>
#define SHELL_INPUT_BUFF_SZ \
	OPTION_MODULE_GET(embox__cmd__sh__shell_registry, NUMBER, input_buff_sz)

typedef void (*shell_run_ft)(void);
typedef int  (*shell_exec_ft)(const char *line);

struct shell {
	const char *  name;
	shell_run_ft  run;
	shell_exec_ft exec;
};

static inline int shell_run(const struct shell *shell) {
	if (shell == NULL) {
		return -EINVAL;
	}

	if (shell->run == NULL) {
		return -ENOSYS;
	}

	shell->run();

	return 0;
}

static inline int shell_exec(const struct shell *shell, const char *line) {
	int ret;

	if (shell == NULL) {
		return -EINVAL;
	}

	if (shell->exec == NULL) {
		return -ENOSYS;
	}

	ret = shell->exec(line);

	if (ret)
		log_error("Shell exec error #%d", ret);

	return ret;
}

extern const struct shell * shell_lookup(const char *shell_name);

extern const struct shell * shell_any(void);

#define SHELL_DEF(...) \
	ARRAY_SPREAD_DECLARE(const struct shell, __shell_registry); \
	ARRAY_SPREAD_ADD(__shell_registry, __VA_ARGS__)

#ifdef __CDT_PARSER__

# undef  SHELL_DEF
# define SHELL_DEF(...) \
	static struct shell MACRO_GUARD(shell) = __VA_ARGS__

#endif /* __CDT_PARSER__ */

#endif /* CMD_SHELL_H_ */
