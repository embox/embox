/**
 * @file
 * @brief Interrupt for ctrl-f.
 *
 * @date 12.04.2010
 * @author Alexey Kryachko
 * @author Maxim Okhotsky
 */

#include <kernel/job.h>
#include <errno.h>

static jmp_buf current_job;

#define LONGJMP_ABORT 1

int job_exec(exec_handler_t exec, int argsc, char **argsv) {
	if (exec == NULL) {
		return -EINVAL;
	}
	if (setjmp(current_job) == 0) {
		return exec(argsc,argsv);
	} else {
		return -EINTR;
	}
}

void job_abort(int i) {
	// TODO not always calling longjmp. -- Alexey
	longjmp(current_job, LONGJMP_ABORT);
}
