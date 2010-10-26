/**
 * @file
 * @brief Interrupt for ctrl-f.
 *
 * @date 12.04.2010
 * @author Alexey Kryachko
 * @author Maxim Okhotsky
 */

#ifndef JOB_H_
#define JOB_H_
#include <setjmp.h>

/**
 * Call the action of command with the restoration point
 *
 * @param calling command with arguments
 * @param first argument of command
 * @param second argument of command
 *
 * @return the activation result
 * @retval 0 if the activation is done
 * @retval -EINVAL if the void doesn't represent valid command
 * @retval -EINTR if the setjump doesn't return 0
 */
extern int job_exec(int (*exec)(int argsc, char **argsv), int argsc, char **argsv);

/**
 * Restore the point after ctrl+f
 */
extern void job_abort(int);

#endif /* JOB_H_ */
