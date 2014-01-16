/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */
#include <unistd.h>
#include <setjmp.h>

extern pid_t kfork(jmp_buf env);

pid_t fork(void) {
	jmp_buf env;

	switch(setjmp(env)) {
	case 0:
		return kfork(env);
	default:
		return 0;
	}
}
