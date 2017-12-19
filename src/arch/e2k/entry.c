#include <asm/io.h>

static int last_cpuid = 0;

__attribute__ ((__section__(".e2k_entry")))
void e2k_entry(void) {
	int cpuid;

	e2k_wait_all();

	cpuid = __e2k_atomic32_add(1, &last_cpuid);

	if (cpuid > 1) {
		/* XXX currently we support only single core */
		while(1);
	}

	kernel_start();
}
