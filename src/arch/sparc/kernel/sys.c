/**
 * @file
 *
 * @date 23.11.2009
 * @author: Anton Bondarev
 */

#include "types.h"
#include "kernel/sys.h"

#if 0
void context_restore(CPU_CONTEXT * pcontext) {

}

static CPU_CONTEXT context;
#endif
volatile static bool started = false;

int sys_exec_start(EXEC_FUNC f, int argc, char **argv) {
	int ret = -2;

	if (started) {
		return -3;
	}

	//context_save(&context);

	if (!started) {
		started = true;
		ret = f(argc, argv);
	}
	started = false;

	return ret;
}

bool sys_exec_is_started(){
	return 0;
}

void sys_exec_stop() {
	if (!started) {
		return;
	}
#if 0
	context_restore(&context);
#endif
}

void sys_halt() {
	//todo must be trap
}


#if 0
/**
 * disable interrupt for atomic operation
 * return old psr reg
 */
unsigned long local_irq_save(void) {
	return 0;
}

/**
 * set PSR_PIL in 0xF
 */
void local_irq_enable(void){
}

/**
 * restore PSR_PIL after __local_irq_save
 * param psr which was returned __local_irq_save
 */
void local_irq_restore(unsigned long old_psr){
}

#endif
