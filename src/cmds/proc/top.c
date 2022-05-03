/**
 * @file
 * @brief
 *
 * @date 14.03.13
 * @author Ilia Vaprol
 */

#include <stdio.h>
#include <kernel/task.h>
#include <kernel/task/task_table.h>
#include <kernel/task/resource/u_area.h>

int main(int argc, char **argv) {
	struct task *task;

	printf("PID USER  PR COMMAND\n");

	task_foreach(task) {
		printf("%-3d %-4d % 3d %s\n", tid,
				task_resource_u_area(task)->reuid,
				task_get_priority(task), task_get_name(task));
	}

	return 0;
}
