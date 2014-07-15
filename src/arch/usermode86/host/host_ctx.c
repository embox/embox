/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.12.2013
 */

#include "host_defs.h"
#include <assert.h>
#include <ucontext.h>

#include <host.h>

HOST_FNX(void, makecontext, CONCAT(void *ucp, void (*func)(), int argc),
		ucp, func, argc)

HOST_FNX(int, swapcontext, CONCAT(void *oucp, void *ucp),
		oucp, ucp)

HOST_FNX(int, getcontext, void *ucp,
		ucp)

void host_stackcontext(void *_ucp, void *sp, int size) {
	ucontext_t *ucp = _ucp;

	ucp->uc_stack.ss_sp = sp;
	ucp->uc_stack.ss_size = size;
}
