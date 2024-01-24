/**
 * @file
 * @brief The external API for accessing available cmds.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_API_H_
#define FRAMEWORK_CMD_API_H_

#include <lib/libds/array.h>
#include <framework/mod/api.h>

#include <framework/cmd/types.h>

#define cmd_foreach(cmd) \
	array_spread_foreach(cmd, __cmd_registry)

ARRAY_SPREAD_DECLARE(const struct cmd * const, __cmd_registry);

#define CMD_ADD(_cmd_ptr) \
	ARRAY_SPREAD_DECLARE(const struct cmd * const, __cmd_registry); \
	ARRAY_SPREAD_ADD(__cmd_registry, _cmd_ptr)

static inline const char *cmd_name(const struct cmd *cmd) {
	return cmd->desc ? cmd->desc->name : NULL;
}

static inline const char *cmd_brief(const struct cmd *cmd) {
	return cmd->desc ? cmd->desc->brief : NULL;
}

static inline const char *cmd_details(const struct cmd *cmd) {
	return cmd->desc ? cmd->desc->details : NULL;
}

struct cmd;

extern int cmd_exec(const struct cmd *cmd, int argc, char **argv);

extern const struct cmd *cmd_lookup(const char *name);

#endif /* FRAMEWORK_CMD_API_H_ */
