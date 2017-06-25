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

int getgroups(int gidsetsize, gid_t grouplist[]) {
	fprintf(stderr, ">>>%s\n", __func__);
	return -ENOSYS;
}



