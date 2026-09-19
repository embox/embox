/**
 * @file
 * @brief Logical CPU ids (aarch64).
 *
 * The id is kept in TPIDR_EL1, one `mrs` per read: cpu_get_id() is on the
 * critical_enter() path, through cpudata_var(). The reset handler writes the
 * register, whose reset value is UNKNOWN, before anything can ask.
 */

#include <stdint.h>

#include <hal/cpu.h>

#include <aarch64/cpu_id.h>

/* Nothing else in the port touches TPIDR_EL1 */
#define TPIDR_EL1_GET()                                             \
	({                                                              \
		uint64_t __v;                                               \
		__asm__ __volatile__("mrs %0, tpidr_el1" : "=r"(__v));      \
		__v;                                                        \
	})

#define TPIDR_EL1_SET(v) \
	__asm__ __volatile__("msr tpidr_el1, %0" : : "r"((uint64_t)(v)))

#define MPIDR_EL1_GET()                                             \
	({                                                              \
		uint64_t __v;                                               \
		__asm__ __volatile__("mrs %0, mpidr_el1" : "=r"(__v));      \
		__v;                                                        \
	})

/* MPIDR of each logical id, shared: a core writes only its own slot */
static uint64_t cpu_mpidr[NCPU];

uint64_t aarch64_mpidr(void) {
	return MPIDR_EL1_GET() & MPIDR_AFF_MASK;
}

unsigned int cpu_get_id(void) {
	return (unsigned int)TPIDR_EL1_GET();
}

void aarch64_cpu_id_register(unsigned int cpu_id) {
	if (cpu_id >= NCPU) {
		return;
	}

	TPIDR_EL1_SET(cpu_id);
	cpu_mpidr[cpu_id] = aarch64_mpidr();
}

uint64_t aarch64_cpu_mpidr(unsigned int cpu_id) {
	return (cpu_id < NCPU) ? cpu_mpidr[cpu_id] : 0;
}
