/**
 * @file
 * @brief Type declarations common for each command and external API.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_TYPES_H_
#define FRAMEWORK_CMD_TYPES_H_

#include <framework/mod/types.h>
#include <util/member.h>

/**
 * Each command implements this interface.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
typedef int (*cmd_exec_t)(int argc, char **argv);

struct cmd_desc {
	const char *name;
	const char *brief;
	const char *details;
};

struct cmd {
	cmd_exec_t exec;
	const struct cmd_desc *desc;
};

struct cmd_mod {
	struct mod mod;
	struct cmd cmd;
	const struct mod_app *app;
};

static inline const struct mod *cmd2mod(const struct cmd *cmd) {
	return &member_cast_out(cmd, const struct cmd_mod, cmd)->mod;
}

#endif /* FRAMEWORK_CMD_TYPES_H_ */
