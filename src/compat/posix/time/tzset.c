/**
 * @file
 * @brief
 *
 * @date 30.04.25
 * @author Anton Bondarev
 */

#include <time.h>

char *tzname[2];

void tzset(void) {
	// http://www.gnu.org/software/libc/manual/html_node/Time-Zone-Functions.html
	tzname[0] = "EST";
	tzname[1] = "EDT";
}
