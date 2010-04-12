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

extern int job_push();

extern void job_abort();

#endif /* JOB_H_ */
