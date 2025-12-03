/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.12.25
 */

#include <sys/types.h>
#include <unistd.h>

#include <kernel/printk.h>

pid_t setsid(void) {
	printk(">>> %s\n", __func__);
	return -1;
}
