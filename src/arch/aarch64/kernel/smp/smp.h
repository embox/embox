/**
 * @file
 * @brief Inter-processor interrupts on aarch64.
 */
#ifndef AARCH64_SMP_H_
#define AARCH64_SMP_H_

#ifndef __ASSEMBLER__

#include <sys/cdefs.h>

/** The SGI that asks another core to reschedule. Zero, as in Linux. */
#define AARCH64_SGI_RESCHED 0

/** The SGI that asks another core to report where it is and stop. Sent by a
 * core on its way into a panic or a failed assertion. */
#define AARCH64_SGI_STOP    1

__BEGIN_DECLS

/** Raise the reschedule IPI on another CPU. Called by the scheduler. */
extern void smp_send_resched(int cpu_id);

/**
 * How many reschedule IPIs a CPU has taken since it came up. Monotonic, so a
 * caller compares it against itself over an interval.
 */
extern unsigned long aarch64_smp_ipi_count(unsigned int cpu_id);

/**
 * Halt every other CPU and collect what each was doing. Called from the abort
 * path: the waits are bounded, no lock is taken, and only the first caller
 * does anything. Declared in <hal/cpu.h> too, for generic code.
 */
extern void smp_stop_others(void);

/** Print what smp_stop_others() collected. Prints once. */
extern void smp_print_stopped(void);

/**
 * Report the interrupted context and park. Called from the interrupt entry
 * when AARCH64_SGI_STOP arrives, before any lock is taken; never returns.
 */
extern void aarch64_smp_stop_self(unsigned long pc, unsigned long lr,
    unsigned long sp, unsigned long psr);

__END_DECLS

#endif /* !__ASSEMBLER__ */

#endif /* AARCH64_SMP_H_ */
