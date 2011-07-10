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

#include "types.h"

#define __cmd_foreach(cmd) \
	array_foreach_ptr(cmd, __cmd_registry, ARRAY_SPREAD_SIZE(__cmd_registry))

extern const struct cmd __cmd_registry[];

static inline const char *cmd_name(const struct cmd *cmd) {
	return NULL != cmd ? cmd->mod->name : NULL;
}

static inline const char *cmd_brief(const struct cmd *cmd) {
	return NULL != cmd ? cmd->mod->brief : NULL;
}

static inline const char *cmd_details(const struct cmd *cmd) {
	return NULL != cmd ? cmd->mod->details : NULL;
}

#endif /* FRAMEWORK_CMD_API_IMPL_H_ */
