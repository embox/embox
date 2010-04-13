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

extern int job_exec(int (*exec)(int argsc, char **argsv), int argsc, char **argsv);
extern void job_abort(void);

#endif /* JOB_H_ */
