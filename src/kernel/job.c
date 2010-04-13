/**
 * @file
 * @brief Interrupt for ctrl-f.
 *
 * @date 12.04.2010
 * @author Alexey Kryachko
 * 			Maxim Okhotsky
 *
 */

#include <kernel/job.h>

static jmp_buf current_job;
static int current_adr;

//Setting up a point for longjmp if the job would be interrupted by pushing ctrl-f
int job_exec(SHELL_COMMAND_DESCRIPTOR *descriptor, int argsc,
		char **argsv) {
	current_adr = setjmp(current_job);
	return descriptor->exec(argsc,argsv);
}

void job_abort(){
	longjmp(current_job, current_adr);
}
