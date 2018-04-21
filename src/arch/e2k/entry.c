#include <string.h>

#include <asm/io.h>

static int last_cpuid = 0;

void kernel_start(void);

__attribute__ ((__section__(".e2k_entry")))
void e2k_entry(void) {
	int cpuid;
	extern char *_t_entry;

	e2k_wait_all();

	cpuid = __e2k_atomic32_add(1, &last_cpuid);

	if (cpuid > 1) {
		/* XXX currently we support only single core */
		while(1);
	}

	/* copy of trap table */
	memcpy((void*)0, &_t_entry, 0x1800);

	kernel_start();
}
