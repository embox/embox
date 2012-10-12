/**
 * @file
 * @brief XXX this is not arch code but platfrom at91sam7
 *
 * @date 21.06.10
 * @author Anton Kozlov
 */

#include <hal/reg.h>
#include <drivers/watchdog.h>
#include <drivers/at91sam7s256.h>

void arch_init(void) {

}

void arch_idle(void) {
}

void arch_shutdown(void) {
	while (1);
}
