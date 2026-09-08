/**
 * @file
 * @brief PSCI calls used to start a secondary core (aarch64).
 *
 * The conduit comes from the psci_method option and has to agree with how the
 * machine was started: an image booted as a plain ELF is handed no device
 * tree to read /psci/method from. A board without PSCI sets it to "".
 */

#include <stdint.h>
#include <string.h>

#include <framework/mod/options.h>
#include <util/log.h>

#include <aarch64/psci.h>

#define PSCI_METHOD OPTION_STRING_GET(psci_method)

/* PSCI 0.2+, SMC Calling Convention: bit 30 selects the 64-bit variant. */
#define PSCI_FN_VERSION 0x84000000UL
#define PSCI_FN_CPU_ON  0xc4000003UL

enum psci_conduit {
	PSCI_CONDUIT_NONE,
	PSCI_CONDUIT_SMC,
	PSCI_CONDUIT_HVC,
};

static enum psci_conduit conduit(void) {
	if (!strcmp(PSCI_METHOD, "smc")) {
		return PSCI_CONDUIT_SMC;
	}
	if (!strcmp(PSCI_METHOD, "hvc")) {
		return PSCI_CONDUIT_HVC;
	}
	return PSCI_CONDUIT_NONE;
}

static long psci_call(unsigned long fn, unsigned long a1, unsigned long a2,
    unsigned long a3) {
	/* conduit() is called before x0..x3 are bound: a call in between would
	 * clobber them */
	const enum psci_conduit c = conduit();

	if (c == PSCI_CONDUIT_NONE) {
		return PSCI_NOT_SUPPORTED;
	}

	{
		register unsigned long x0 __asm__("x0") = fn;
		register unsigned long x1 __asm__("x1") = a1;
		register unsigned long x2 __asm__("x2") = a2;
		register unsigned long x3 __asm__("x3") = a3;

		if (c == PSCI_CONDUIT_SMC) {
			__asm__ __volatile__("smc #0"
			                     : "+r"(x0)
			                     : "r"(x1), "r"(x2), "r"(x3)
			                     : "memory");
		}
		else {
			__asm__ __volatile__("hvc #0"
			                     : "+r"(x0)
			                     : "r"(x1), "r"(x2), "r"(x3)
			                     : "memory");
		}

		return (long)x0;
	}
}

bool psci_available(void) {
	return conduit() != PSCI_CONDUIT_NONE;
}

const char *psci_method(void) {
	return PSCI_METHOD[0] ? PSCI_METHOD : "(none)";
}

long psci_version(void) {
	return psci_call(PSCI_FN_VERSION, 0, 0, 0);
}

long psci_cpu_on(uint64_t mpidr, uintptr_t entry, uint64_t context_id) {
	return psci_call(PSCI_FN_CPU_ON, mpidr, entry, context_id);
}
