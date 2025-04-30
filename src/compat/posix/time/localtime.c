/**
 * @file
 * @brief
 *
 * @date 30.04.25
 * @author Anton Bondarev
 */

#include <time.h>

int daylight = 0;
long timezone = 0;

struct tm *localtime_r(const time_t *timep, struct tm *result) {
	//TODO local time not use timezone now
	return gmtime_r(timep, result);
}

struct tm *localtime(const time_t *timep) {
	//TODO local time not use timezone now
	return gmtime(timep);
}
