/**
 * @file
 *
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 12.05.12
 * @author Ilia Vaprol
 * @author Anton Bondarev
 *         - clock()
 */

#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include <kernel/time/time.h>

time_t time(time_t *t) {
	struct timespec ts;

	getnsofday(&ts, NULL);

	if (t != NULL) {
		*t = ts.tv_sec;
	}

	return ts.tv_sec;
}
