/**
 * @file
 * @brief
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <drivers/char_dev.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>
#include <util/err.h>
#include <util/log.h>

const char *setup_tty(const char *dev_name) {
	struct char_dev *cdev;
	struct idesc_table *it;
	struct idesc *idesc;
	int fd;

	cdev = char_dev_find(dev_name);
	if (!cdev) {
		log_error("tty device not found");
		return NULL;
	}

	idesc = char_dev_open(cdev, O_RDWR);
	if (ptr2err(idesc)) {
		log_error(strerror(-ptr2err(idesc)));
		return NULL;
	}

	it = task_resource_idesc_table(task_self());
	fd = idesc_table_add(it, idesc, 0);
	if (fd < 0) {
		return NULL;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > 2) {
		close(fd);
	}

	return dev_name;
}
