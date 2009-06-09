/**
 * \file sys.c
 *
 * \date Jun 09, 2009
 * \author Eldar Abusalimov
 * \author Alexey Fomin
 */

#include "sys.h"
#include "cpu_context.h"
#include "types.h"

inline static void context_save(CPU_CONTEXT *pcontext) {
	__asm__ __volatile__(
			"mov %0, %%o0\n"
			"ta 5\n" :
			"=r" (pcontext)::"%o0");
}

inline static void context_restore(CPU_CONTEXT *pcontext) {
	__asm__ __volatile__(
			"mov %0, %%o0\n"
			"ta 6\n" :
			"=r" (pcontext));
}

static CPU_CONTEXT context;

int sys_exec(EXEC_FUNC f, int argc, char **argv) {
	static BOOL started = FALSE;
	int ret = -2;

	context_save(&context);
	{
		if (!started) {
			started = TRUE;
			ret = f(argc, argv);
		} else {
			return ret;
		}
	}
	context_restore(&context);

	// we'll never reach this line
	return ret;
}

void sys_interrupt() {
	context_restore(&context);
}

//400172dc <context_save>:
//400172dc:	9d e3 bf 98 	save  %sp, -104, %sp
//400172e0:	f0 27 a0 44 	st  %i0, [ %fp + 0x44 ]
//400172e4:	90 10 00 01 	mov  %g1, %o0
//400172e8:	91 d0 20 05 	ta  5
//400172ec:	c2 27 a0 44 	st  %g1, [ %fp + 0x44 ]
//400172f0:	81 c7 e0 08 	ret
//400172f4:	81 e8 00 00 	restore
//
//400172f8 <context_restore>:
//400172f8:	9d e3 bf 98 	save  %sp, -104, %sp
//400172fc:	f0 27 a0 44 	st  %i0, [ %fp + 0x44 ]
//40017300:	90 10 00 01 	mov  %g1, %o0
//40017304:	91 d0 20 06 	ta  6
//40017308:	c2 27 a0 44 	st  %g1, [ %fp + 0x44 ]
//4001730c:	81 c7 e0 08 	ret
//40017310:	81 e8 00 00 	restore
