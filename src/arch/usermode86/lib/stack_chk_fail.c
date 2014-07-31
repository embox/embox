/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 05.03.13
 */

#include <kernel/printk.h>

void __stack_chk_fail(void) {
	printk("stack overflow\n");
	while (1) {}
}
