/**
 * @file
 * @brief CGI script to show tasks
 *
 * @date 29.01.2021
 * @author Alexander Kalmuk
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/task.h>
#include <kernel/task/task_table.h>
#include <kernel/task/resource/u_area.h>

int main(int argc, char *argv[]) {
	char *buf, *pbuf;
	struct task *task;

	buf = malloc(512);
	if (!buf) {
		fprintf(stderr, "%s: Failed allocate buffer\n", argv[0]);
		return -1;
	}

	printf(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
	);

	while (1) {
		pbuf = buf;

		pbuf += sprintf(pbuf, "data: PID USER  PR COMMAND\n");

		task_foreach(task) {
			pbuf += sprintf(pbuf, "data: %-3d %-4d % 3d %s\n", tid,
			                task_resource_u_area(task)->reuid,
			                task_get_priority(task), task_get_name(task));
		}
		sprintf(pbuf, "\n");

		if (0 > printf("%s", buf)) {
			break;
		}

		sleep(1);
	}

	free(buf);

	return 0;
}
