/**
 * @file
 *
 * @details
 *   Implements the following functions from the Base Definitions volume
 *   of IEEE Std 1003.1-2001:
 *     - sigaddset()
 *     - sigdelset()
 *     - sigemptyset()
 *     - sigfillset()
 *     - sigismember()
 *     - sigprocmask()
 *
 * @date 22.10.2013
 * @author Eldar Abusalimov
 */


#include <errno.h>
#include <signal.h>

#include <lib/libds/bitmap.h>
#include <util/math.h>

int sigemptyset(sigset_t *set)
{
	bitmap_clear_all(set->bitmap, _SIG_TOTAL);
	return 0;
}

int sigfillset(sigset_t *set)
{
	bitmap_set_all(set->bitmap, _SIG_TOTAL);
	return 0;
}

int sigismember(const sigset_t *set, int signo)
{
	if (!check_range(signo, 1, _SIG_TOTAL))  /* signal 0 is reserved */
		return -EINVAL;

	return bitmap_test_bit(set->bitmap, signo);
}

int sigaddset(sigset_t *set, int signo)
{
	if (!check_range(signo, 1, _SIG_TOTAL))
		return -EINVAL;

	bitmap_set_bit(set->bitmap, signo);
	return 0;
}

int sigdelset(sigset_t *set, int signo)
{
	if (!check_range(signo, 1, _SIG_TOTAL))
		return -EINVAL;

	bitmap_clear_bit(set->bitmap, signo);
	return 0;
}


