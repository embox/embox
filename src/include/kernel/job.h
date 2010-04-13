/**
 * @file
 * @brief Interrupt for ctrl-f.
 *
 * @date 12.04.2010
 * @author Alexey Kryachko
 * 			Maxim Okhotsky
 *
 */

#ifndef JOB_H_
#define JOB_H_
#include <setjmp.h>
#include <shell_command.h>

extern int job_exec(SHELL_COMMAND_DESCRIPTOR *descriptor, int argsc, char **argsv);
extern void job_abort(void);

#endif /* JOB_H_ */
