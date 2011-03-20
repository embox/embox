/**
 * @file
 * @brief Type declarations common for each command and external API.
 *
 * @date Mar 1, 2011
 * @author Eldar Abusalimov
 */

#ifndef IMPL_CMD_TYPES_H_
#define IMPL_CMD_TYPES_H_

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


#endif /* IMPL_CMD_TYPES_H_ */
