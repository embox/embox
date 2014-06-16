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

#if 0
pid_t fork(void) {
	return -1;
}
#endif

pid_t vfork(void) {
	jmp_buf env;

	switch(setjmp(env)) {
	case 0:
		return kfork(env);
	default:
		return 0;
	}
}
