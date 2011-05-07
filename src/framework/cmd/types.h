/**
 * @file
 * @brief Type declarations common for each command and external API.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#ifndef FRAMEWORK_CMD_TYPES_H_
#define FRAMEWORK_CMD_TYPES_H_

/**
 * Each command implements this interface.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
typedef int (*cmd_exec_t)(int argc, char **argv);

struct cmd {
	cmd_exec_t exec;
	/** The corresponding mod. */
	const struct mod *mod;
};


#endif /* FRAMEWORK_CMD_TYPES_H_ */
