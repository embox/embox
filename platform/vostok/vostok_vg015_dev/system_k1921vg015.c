
#include <stdint.h>

#include <drivers/clk/k1921vg015_rcu.h>

#include <system_k1921vg015.h>

void SystemInit(void) {
    niiet_sysclk_init();
}
