/**
 * @file
 *
 * @brief
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <kernel/task.h>

void _exit(int status) {
	task_exit(NULL);
}

/* stdlib */
void exit(int status) {
	_exit(status);
}
