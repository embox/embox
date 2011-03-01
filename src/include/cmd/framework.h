/**
 * @file
 * @brief The external API for accessing available cmds.
 *
 * @date Mar 1, 2011
 * @author Eldar Abusalimov
 */

#ifndef CMD_FRAMEWORK_H_
#define CMD_FRAMEWORK_H_

#include <impl/cmd/framework.h>

#define cmd_foreach(cmd) \
		__cmd_foreach(cmd)

/**
 * TODO docs. -- Eldar
 */
struct cmd;

extern int cmd_exec(const struct cmd *cmd, int argc, char **argv);

extern const char *cmd_name(const struct cmd *cmd);

extern const char *cmd_brief(const struct cmd *cmd);

extern const char *cmd_details(const struct cmd *cmd);

#endif /* CMD_FRAMEWORK_H_ */
