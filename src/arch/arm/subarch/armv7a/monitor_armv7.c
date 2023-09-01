#include <inttypes.h>

uint32_t GLOBAL_LR1 = 0;
uint32_t GLOBAL_SP1 = 0;
uint32_t GLOBAL_CPSR1 = 0;
uint32_t GLOBAL_SPSR1 = 0;

uint32_t GLOBAL_LR2 = 0;
uint32_t GLOBAL_SP2 = 0;
uint32_t GLOBAL_CPSR2 = 0;
uint32_t GLOBAL_SPSR2 = 0;

void mon_swi_handler(void) {
	while (1)
		;
}

void mon_func(void) {
}
