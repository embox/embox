/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.12.2013
 */

#include <kernel/task.h>

#include <unistd.h>

pid_t getpid(void) {
	return task_get_id(task_self());
}
