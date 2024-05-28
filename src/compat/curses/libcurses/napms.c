/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <unistd.h>

#include "curses_priv.h"

int napms(int ms) {
	usleep((useconds_t)ms * 1000);
	return OK;
}
