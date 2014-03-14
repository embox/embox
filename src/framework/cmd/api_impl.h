/**
 * @file
 * @brief Implementation of cmd framework getters and iterators.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_API_IMPL_H_
#define FRAMEWORK_CMD_API_IMPL_H_

#include <stddef.h>

#include <util/array.h>
#include <framework/mod/api.h>

#include <framework/cmd/types.h>

#define __cmd_foreach(cmd) \
	array_spread_foreach(cmd, __cmd_registry)

ARRAY_SPREAD_DECLARE(const struct cmd *, __cmd_registry);

#define CMD_ADD(_cmd_ptr) \
	ARRAY_SPREAD_DECLARE(const struct cmd *, __cmd_registry); \
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

#endif /* FRAMEWORK_CMD_API_IMPL_H_ */
