#include <kernel/task/resource/errno.h>

int *__errno(void) {
	return task_self_resource_errno();
}
