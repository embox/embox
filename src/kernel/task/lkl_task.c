#include <embox/unit.h>

EMBOX_UNIT_INIT(lkl_task_init);

static int lkl_task_init(void) {
	printk("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	return 0;
}
