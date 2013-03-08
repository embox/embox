/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 05.03.13
 */

#include <prom/prom_printf.h>

void __stack_chk_fail(void) {
	prom_printf("stack overflow\n");
	while (1) {}
}
