/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.05.2014
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

pid_t getpgrp(void) {
	fprintf(stderr, ">>>%s\n", __func__);
	return -ENOSYS;
}


