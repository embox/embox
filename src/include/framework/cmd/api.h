/**
 * @file
 * @brief The external API for accessing available cmds.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_API_H_
#define FRAMEWORK_CMD_API_H_

#include __impl_x(framework/cmd/api_impl.h)

#define cmd_foreach(cmd) \
		__cmd_foreach(cmd)

/**
 * TODO docs. -- Eldar
 */
struct cmd;

extern int cmd_exec(const struct cmd *cmd, int argc, char **argv);

extern const struct cmd *cmd_lookup(const char *name);

extern const char *cmd_name(const struct cmd *cmd);

extern const char *cmd_brief(const struct cmd *cmd);

extern const char *cmd_details(const struct cmd *cmd);

#endif /* FRAMEWORK_CMD_API_H_ */
