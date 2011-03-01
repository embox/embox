/**
 * @file
 * @brief TODO
 *
 * @date Mar 1, 2011
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_CMD_H_
#define EMBOX_CMD_H_

#include <stddef.h>

#include <util/array.h>
#include <mod/self.h>

#define EMBOX_CMD(exec, brief, detatils) \
		__EMBOX_CMD(exec, brief, detatils)

#define __EMBOX_CMD(_exec, brief, detatils) \
	static int _exec(int argc, char **argv);           \
	ARRAY_SPREAD_ADD_NAMED(__cmd_registry, __cmd,   {  \
			.exec = _exec,                             \
			.mod = &mod_self                           \
		});                                            \
	MOD_SELF_BIND(__cmd, NULL) /* TODO not used. -- Eldar */

#define cmd_foreach(cmd) \
		array_spread_foreach_ptr(cmd, __cmd_registry)

/**
 * Each command implements this interface.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
typedef int(*cmd_exec_t)(int argc, char **argv);

struct cmd {
	cmd_exec_t exec;
	/** The corresponding mod. */
	const struct mod *mod;
};

extern const struct cmd __cmd_registry[];

// XXX for struct mod. -- Eldar
#include <mod/core.h>
inline static const char *cmd_name(const struct cmd *cmd) {
	return NULL != cmd ? cmd->mod->name : NULL;
}
inline static const char *cmd_brief(const struct cmd *cmd) {
	return NULL != cmd ? "Not yet implemented" : NULL;
}
inline static const char *cmd_details(const struct cmd *cmd) {
	return NULL != cmd ? "Not yet implemented" : NULL;
}

#endif /* EMBOX_CMD_H_ */
