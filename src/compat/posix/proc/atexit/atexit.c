/**
 * @file
 * @brief
 *
 * @date 05.8.2020
 * @author  Erick Cafferata
 */

#include <kernel/task/resource/atexit.h>
#include <limits.h>

void __atexit_funcs(void)
{
	void (*atexit_func)(void);
	struct task_atexit *list = task_self_resource_atexit();

	while (list->slot-- > 0) {
		atexit_func = list->atexit_func[list->slot];
		atexit_func();
	}
}

int atexit(void (*func)(void)) {
	struct task_atexit *list = task_self_resource_atexit();

	if (list->slot == ATEXIT_MAX) {
		return -1;
	}

	list->atexit_func[list->slot] = func;
	list->slot++;
	return 0;
}
