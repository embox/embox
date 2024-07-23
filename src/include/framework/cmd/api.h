/**
 * @file
 * @brief The external API for accessing available cmds.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_API_H_
#define FRAMEWORK_CMD_API_H_

#include <sys/cdefs.h>

#include <framework/cmd/types.h>
#include <framework/mod/api.h>
#include <lib/libds/array.h>

#define CMD_REGISTER(_cmd_ptr)                                     \
	ARRAY_SPREAD_DECLARE(const struct cmd *const, __cmd_registry); \
	ARRAY_SPREAD_ADD(__cmd_registry, _cmd_ptr)

#define cmd_foreach(cmd) array_spread_foreach(cmd, __cmd_registry)

ARRAY_SPREAD_DECLARE(const struct cmd *const, __cmd_registry);

__BEGIN_DECLS

static inline const char *cmd_name(const struct cmd *cmd) {
	return cmd->desc ? cmd->desc->name : NULL;
}

static inline const char *cmd_brief(const struct cmd *cmd) {
	return cmd->desc ? cmd->desc->brief : NULL;
}

static inline const char *cmd_details(const struct cmd *cmd) {
	return cmd->desc ? cmd->desc->details : NULL;
}

extern int cmd_exec(const struct cmd *cmd, int argc, char **argv);

extern const struct cmd *cmd_lookup(const char *name);

__END_DECLS

#endif /* FRAMEWORK_CMD_API_H_ */
