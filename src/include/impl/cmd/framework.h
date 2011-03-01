/**
 * @file
 * @brief TODO
 *
 * @date Mar 1, 2011
 * @author Eldar Abusalimov
 */

#ifndef CMD_FRAMEWORK_H_
# error "Do not include this file directly, use <cmd/framework.h> instead!"
#endif /* CMD_FRAMEWORK_H_ */

#include <stddef.h>

#include <util/array.h>
#include <mod/core.h>

#include <impl/cmd/types.h>

#define __cmd_foreach(cmd) \
		array_spread_foreach_ptr(cmd, __cmd_registry)

extern const struct cmd __cmd_registry[];

inline static const char *cmd_name(const struct cmd *cmd) {
	return NULL != cmd ? cmd->mod->name : NULL;
}
inline static const char *cmd_brief(const struct cmd *cmd) {
	return NULL != cmd ? "(cmd_brief) Not yet implemented" : NULL;
}
inline static const char *cmd_details(const struct cmd *cmd) {
	return NULL != cmd ? "(cmd_details) Not yet implemented" : NULL;
}

