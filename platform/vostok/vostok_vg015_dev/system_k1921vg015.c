
#include <stdint.h>

#include <drivers/clk/k1921vg015_rcu.h>

void SystemInit(void) {
    niiet_sysclk_init();
}
