/**
 * @file
 *
 * @date: Jul 18, 2022
 * @author Anton Bondarev
 */
#include <errno.h>
#include <signal.h>

int sigwait(const sigset_t */*restrict*/ set, int */*restrict*/ sig) {
	(void)set; (void) sig;
	return -ENOSYS;
}
